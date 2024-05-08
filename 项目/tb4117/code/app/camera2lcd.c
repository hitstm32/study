#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <linux/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <linux/fb.h>
#include <jpeglib.h>
#include <setjmp.h>


int fd_fb;
int screen_size;//屏幕像素大小
int LCD_width;//LCD宽度
int LCD_height;//LCD高度
unsigned char *fbbase = NULL;//LCD显存地址
unsigned long line_length;       //LCD一行的长度（字节为单位）
unsigned int bpp;    //像素深度bpp


struct my_error_mgr {
  struct jpeg_error_mgr pub;  /* "public" fields */
 
  jmp_buf setjmp_buffer;    /* for return to caller */
};
 
typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
 
  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);
 
  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


int LCD_Init(void)
{
	struct fb_var_screeninfo var;   /* Current var */
	struct fb_fix_screeninfo fix;   /* Current fix */
	fd_fb = open("/dev/fb0", O_RDWR);
	if(fd_fb < 0)
	{
		perror("打开LCD失败");
		return -1;
	}
	//获取LCD信息
	ioctl(fd_fb, FBIOGET_VSCREENINFO, &var);//获取屏幕可变信息
	ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix);//获取屏幕固定信息
	//LCD_width  = var.xres * var.bits_per_pixel / 8;
    //pixel_width = var.bits_per_pixel / 8;
    screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
	LCD_width = var.xres;
	LCD_height = var.yres;
	bpp = var.bits_per_pixel;
	line_length = fix.line_length;
	printf("LCD分辨率：%d %d\n",LCD_width, LCD_height);
	printf("bpp: %d\n", bpp);
	fbbase = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);//映射
	if (fbbase == (unsigned char *)-1)
    {
        printf("can't mmap\n");
        return -1;
    }
    memset(fbbase, 0xFF, screen_size);//LCD设置为白色背景
    return 0;
}


int LCD_JPEG_Show(const char *JpegData, int size)
{
	int min_hight = LCD_height, min_width = LCD_width, valid_bytes;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
    unsigned char *ptr_buf = malloc(LCD_height*LCD_width*bpp / 8);
	cinfo.err = jpeg_std_error(&jerr.pub);//错误处理对象与解压对象绑定
    jerr.pub.error_exit = my_error_exit;

    if(setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        free(ptr_buf);
        return 0;
    }
	//创建解码对象
	jpeg_create_decompress(&cinfo);
	//指定解码数据源
	jpeg_mem_src(&cinfo, JpegData, size);
	//读取图像信息
	jpeg_read_header(&cinfo, TRUE);
	//printf("jpeg图像的大小为：%d*%d\n", cinfo.image_width, cinfo.image_height);
	//设置解码参数
	cinfo.out_color_space = JCS_RGB;//可以不设置默认为RGB
	//cinfo.scale_num = 1;
	//cinfo.scale_denom = 1;设置图像缩放，scale_num/scale_denom缩放比例，默认为1
	//开始解码
	jpeg_start_decompress(&cinfo);
	
	//为缓冲区分配空间
	unsigned char*jpeg_line_buf = malloc(cinfo.output_components * cinfo.output_width);
	u_int16_t*  fb_line_buf = malloc(line_length);//每个成员4个字节和RGB565对应
    
	//判断图像和LCD屏那个分辨率更低
	if(cinfo.output_width < min_width)
		min_width = cinfo.output_width;
	if(cinfo.output_height < min_hight)
		min_hight = cinfo.output_height;
	//读取数据，数据按行读取
	valid_bytes = min_width * bpp / 8;//一行的有效字节数，实际写进LCD显存的一行数据大小
	unsigned char *ptr_l = fbbase;
    unsigned int ptr_add=0;
    
    
	while(cinfo.output_scanline < min_hight)
	{
		jpeg_read_scanlines(&cinfo, &jpeg_line_buf, 1);//每次读取一行
		//将读取到的BGR888数据转化为RGB888
		u_int16_t red, green, blue;
		u_int16_t color;  
		for(int i = 0; i < min_width; i++)
		{
			red = jpeg_line_buf[i*3];
			green = jpeg_line_buf[i*3+1];
			blue = jpeg_line_buf[i*3+2];
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			fb_line_buf[i] = color;
		}
		memcpy(ptr_buf+ptr_add, fb_line_buf, valid_bytes);
		ptr_add += LCD_width*bpp/8;
	}
    memcpy(ptr_l, ptr_buf, LCD_height*LCD_width*bpp / 8);

	//完成解码
	jpeg_finish_decompress(&cinfo);
	//销毁解码对象
	jpeg_destroy_decompress(&cinfo);
	//释放内存
	free(jpeg_line_buf);
    free(ptr_buf);
	free(fb_line_buf);
	return 1;
}













int main(int argc, char *argv[])
{
    int fd;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_frmsizeenum fsenum;
    struct v4l2_format fmt;
    struct v4l2_capability cap;
    struct v4l2_buffer buf;

    int fmt_index = 0;
    int fs_index = 0;
    int i=0;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    void *bufs[2];
    int buf_cnt=2;

    struct pollfd  fds[1];

    char filename[32];
    int file_cnt=0;

    LCD_Init();

    // if(argc !=2)
    // {
    //     printf("Usage: %s </dev/videoX>, print format detail for video device\n");
    //     return -1;
    // }

    
    while((fd=open("/dev/video0", O_RDWR))<0)
    {
        printf("can not open /dev/video0, retrying......\n");
        sleep(1);
    }
    
    //判断有无视频获取能力
    memset(&cap, 0, sizeof(cap));
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap)==0)
    {
        if((cap.capabilities&V4L2_CAP_VIDEO_CAPTURE)==0)
        {
            printf("video capture not support!\n");
            return -1;
        }

        if((cap.capabilities&V4L2_CAP_STREAMING)==0)
        {
            printf("video streaming not support!\n");
            return -1;
        }
    }else
    {
        printf("open err!\n");
        return -1;
    }


    while(1)
    {
        //枚举格式
        fmtdesc.index = fmt_index;
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


        if(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
        {
            printf("ioctl stop!\n");
            break;
        }

        fs_index = 0;
        while(1)
        {
            //枚举这种格式支持的帧大小
            memset(&fsenum, 0, sizeof(fsenum));
            fsenum.pixel_format = fmtdesc.pixelformat;
            fsenum.index = fs_index;
            if(ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsenum) == 0)
            {
                printf("format %d :%s, framesize %d: %d x %d\n",fmt_index,fmtdesc.description,fsenum.index,fsenum.discrete.height,fsenum.discrete.width);
                fs_index++;
            }else
            {
                fmt_index++;
                break;
            }
        }
    }


    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 240;
    fmt.fmt.pix.height = 320;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;

    if(ioctl(fd, VIDIOC_S_FMT, &fmt)==0)
    {
        printf("set format ok, real %d x %d \n",fmt.fmt.pix.width,fmt.fmt.pix.height);
    }else
    {
        printf("set format err!\n");
    }


    //request buffer
    struct v4l2_requestbuffers rb;
    memset(&rb, 0, sizeof(rb));
    rb.count = buf_cnt;
    rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rb.memory =V4L2_MEMORY_MMAP;
    if(ioctl(fd, VIDIOC_REQBUFS, &rb)==0)
    {
        //申请成功，进行mmap
        buf_cnt = rb.count;
        for(i=0;i<rb.count;i++)
        {
            memset(&buf, 0, sizeof(buf));
            buf.index=i;
            buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory=V4L2_MEMORY_MMAP;
            if(ioctl(fd, VIDIOC_QUERYBUF, &buf)==0)
            {
                bufs[i] = mmap(0, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
                if(bufs[i]==MAP_FAILED)
                {
                    printf("mmap faild!\n");
                    return -1;
                }
            }else
            {
                printf("query buf faild!\n");
                return -1;
            }
        }
        printf("mmap %d buf\n",buf_cnt);
        
    }else
    {
        printf("request buf err!\n");
        return -1;
    }

    //把所有buffer放入空闲链表，等待被填充
    for(i=0;i<buf_cnt;i++)
    {
        memset(&buf, 0, sizeof(buf));
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_QBUF, &buf)!=0)
        {
            printf("queue buf faild!\n");
            return -1;
        }
    }

    //启动摄像头
    
    if(ioctl(fd, VIDIOC_STREAMON, &type)!=0)
    {
        printf("start camera err!\n");
        return -1;
    }

    while(1)
    {
        //poll
        memset(fds, 0, sizeof(fds));
        fds[0].events = POLLIN;
        fds[0].fd = fd;
        if(poll(fds, 1, -1)==1)
        {
            //buffer取出队列
            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;

            if(ioctl(fd, VIDIOC_DQBUF, &buf)!=0)
            {
                printf("unable to dequeue buf!\n");
                return -1;
            }

        }

        //buffer存为文件
        // sprintf(filename, "video_raw_data%04d.jpg",file_cnt++);
        // int fd_file = open(filename, O_RDWR|O_CREAT, 0666);
        // write(fd_file, bufs[buf.index], buf.bytesused);
        // close(fd_file);
        LCD_JPEG_Show(bufs[buf.index], buf.length);
        //放入队列

        if(ioctl(fd, VIDIOC_QBUF, &buf)!=0)
        {
            printf("unable to queue buf\n");
            return -1;
        }
        //printf("%s save ok\n",filename);

    }

    if(ioctl(fd, VIDIOC_STREAMOFF, &type)!=0)
    {
        printf("off faild\n");
        return -1;
    }


    return 0;
}


