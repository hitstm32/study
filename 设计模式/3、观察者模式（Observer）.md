如果要设计一个文件分割器，需要加入一个进度条，来显示进度，最常规的想法是直接在文件分割器类里加个进度条控件，并在分割文件时setval来实现进度条更换。但是这样就违反了<span style="color:#ffc000">依赖倒置原则</span>，因为顶层代码直接依赖了底层。有问题的代码如下：
```cpp
class MainForm : public Form
{
	TextBox* txtFilePath;
	TextBox* txtFileNumber;
	ProgressBar* progressBar;
public:
	void Button1_Click(){
		string filePath = txtFilePath->getText();
		int number = atoi(txtFileNumber->getText().c_str());
		FileSplitter splitter(filePath, number, progressBar);
		splitter.split();
	}
};

class FileSplitter
{
	string m_filePath;
	int m_fileNumber;
	ProgressBar* m_progressBar;
public:
	FileSplitter(const string& filePath, int fileNumber, ProgressBar* progressBar) :
		m_filePath(filePath), 
		m_fileNumber(fileNumber),
		m_progressBar(progressBar){
	}

	void split(){
		//1.读取大文件
		//2.分批次向小文件中写入
		for (int i = 0; i < m_fileNumber; i++){
			//...
			float progressValue = m_fileNumber;
			progressValue = (i + 1) / progressValue;
			m_progressBar->setValue(progressValue);
		}
	}
};
```
这是常规写法，在这种写法中，算法模块`FileSplitter`中不得不加入了`ProgressBar*`指针，以传递进度条类。但是这样导致了算法类不再纯粹，而是依赖了易变的界面库，导致算法模块不再稳定。
修改：
1.mainform：
```cpp
class MainForm : public Form, public IProgress
{
	TextBox* txtFilePath;
	TextBox* txtFileNumber;

	ProgressBar* progressBar;

public:
	void Button1_Click(){
		string filePath = txtFilePath->getText();
		int number = atoi(txtFileNumber->getText().c_str());
		ConsoleNotifier cn;
		FileSplitter splitter(filePath, number);
		splitter.addIProgress(this); //订阅通知
		splitter.addIProgress(&cn); //订阅通知
		splitter.split();
		splitter.removeIProgress(this);
	}
	virtual void DoProgress(float value){
		progressBar->setValue(value);
	}
};

class ConsoleNotifier : public IProgress {
public:
	virtual void DoProgress(float value){
		cout << ".";
	}
};
```

2：filesplitter：
```cpp
class IProgress{
public:
	virtual void DoProgress(float value)=0;
	virtual ~IProgress(){}
};


class FileSplitter
{
	string m_filePath;
	int m_fileNumber;
	List<IProgress*>  m_iprogressList; // 抽象通知机制，支持多个观察者
public:
	FileSplitter(const string& filePath, int fileNumber) :
		m_filePath(filePath), 
		m_fileNumber(fileNumber){
	}


	void split(){
		//1.读取大文件
		//2.分批次向小文件中写入
		for (int i = 0; i < m_fileNumber; i++){
			//...
			float progressValue = m_fileNumber;
			progressValue = (i + 1) / progressValue;
			onProgress(progressValue);//发送通知
		}

	}


	void addIProgress(IProgress* iprogress){
		m_iprogressList.push_back(iprogress);
	}

	void removeIProgress(IProgress* iprogress){
		m_iprogressList.remove(iprogress);
	}


protected:
	virtual void onProgress(float value){
		
		List<IProgress*>::iterator itor=m_iprogressList.begin();
		while (itor != m_iprogressList.end() )
			(*itor)->DoProgress(value); //更新进度条
			itor++;
		}
	}
};
```
通过以上方式，算法模块和进度条完全解耦，调用者自己继承消息模块，并把消息自己的消息抽象类注册给算法模块，算法模块只需要发送消息给各个类的消息抽象类中的DoProgress函数，即可完成消息传递。各个调用者类只需要实现自己的DoProgress即可。
![[Pasted image 20240730213118.png]]
