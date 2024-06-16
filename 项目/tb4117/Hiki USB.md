对于热成像设备，可调节画面、配置测温规则、校准。
![[Pasted image 20240609205403.png]]
![[Pasted image 20240609205417.png]]
![[Pasted image 20240609205543.png]]
![[Pasted image 20240609205549.png]]
![[Pasted image 20240609205555.png]]
![[Pasted image 20240609205602.png]]
![[Pasted image 20240609205846.png]]
![[Pasted image 20240609205852.png]]

# 图像镜像
```cpp
    case USB_SET_IMAGE_VIDEO_ADJUST:
        int type;
        struCond.byChannelID = 1;
        struConfigInputInfo.lpCondBuffer = &struCond;
        struConfigInputInfo.dwCondBufferSize = sizeof(struCond);
        struConfigOutputInfo.lpOutBuffer = &struImageVideoAdjuse;
        struConfigOutputInfo.dwOutBufferSize = sizeof(struImageVideoAdjuse);
        if (!USB_GetDeviceConfig(m_lUserID, USB_GET_IMAGE_VIDEO_ADJUST, &struConfigInputInfo, &struConfigOutputInfo))
        {
            LOGIN_INFO->WriteLog(ERR_LEVEL, "FAILED  USB_GET_IMAGE_VIDEO_ADJUST");
            DWORD dwError = USB_GetLastError();
            LOGIN_INFO->WriteLog(INF_LEVEL, "ErrorCode = [%d], ErrorMsg = [%s]", dwError, USB_GetErrorMsg(dwError));
        }
        else
        {
            LOGIN_INFO->WriteLog(INF_LEVEL, "SUCCESS USB_GET_IMAGE_VIDEO_ADJUST byImageFlipStyle[%d]", struImageVideoAdjuse.byImageFlipStyle);
        }

        type = struImageVideoAdjuse.byImageFlipStyle;
        type = (type + 1) % 4;
        struImageVideoAdjuse.byImageFlipStyle = type;
        struConfigInputInfo.lpInBuffer = &struImageVideoAdjuse;
        struConfigInputInfo.dwInBufferSize = sizeof(struImageVideoAdjuse);
        if (!USB_SetDeviceConfig(m_lUserID, USB_SET_IMAGE_VIDEO_ADJUST, &struConfigInputInfo, NULL))
        {
            LOGIN_INFO->WriteLog(ERR_LEVEL, "FAILED  USB_SET_ROTATE_ANGLE");
            DWORD dwError = USB_GetLastError();
            LOGIN_INFO->WriteLog(INF_LEVEL, "ErrorCode = [%d], ErrorMsg = [%s]", dwError, USB_GetErrorMsg(dwError));
        }
        else
        {
            LOGIN_INFO->WriteLog(INF_LEVEL, "SUCCESS USB_SET_IMAGE_VIDEO_ADJUST byImageFlipStyle[%d]", struImageVideoAdjuse.byImageFlipStyle);
        }
        break;
```

设置伪彩
```cpp
    case USB_SET_IMAGE_ENHANCEMENT: //设置图像增强参数
        int type;
        struCond.byChannelID = 1;
        struConfigInputInfo.lpCondBuffer = &struCond;
        struConfigInputInfo.dwCondBufferSize = sizeof(struCond);

        struConfigOutputInfo.lpOutBuffer = &struImageEnhancement;
        struConfigOutputInfo.dwOutBufferSize = sizeof(struImageEnhancement);

        if (!USB_GetDeviceConfig(m_lUserID, USB_GET_IMAGE_ENHANCEMENT, &struConfigInputInfo, &struConfigOutputInfo))
        {
            LOGIN_INFO->WriteLog(ERR_LEVEL, "FAILED  USB_GET_IMAGE_ENHANCEMENT");
            DWORD dwError = USB_GetLastError();
            LOGIN_INFO->WriteLog(INF_LEVEL, "ErrorCode = [%d], ErrorMsg = [%s]", dwError, USB_GetErrorMsg(dwError));
        }
        else
        {
            LOGIN_INFO->WriteLog(INF_LEVEL, "SUCCESS  USB_GET_IMAGE_ENHANCEMENT byNoiseReduceMode[%d] dwGeneralLevel[%d] dwFrameNoiseReduceLevel[%d] dwInterFrameNoiseReduceLevel[%d] byPaletteMode[%d] byLSEDetailEnabled[%d] dwLSEDetailLevel[%d]",
                struImageEnhancement.byNoiseReduceMode, struImageEnhancement.dwGeneralLevel, struImageEnhancement.dwFrameNoiseReduceLevel, struImageEnhancement.dwInterFrameNoiseReduceLevel, struImageEnhancement.byPaletteMode, struImageEnhancement.byLSEDetailEnabled, struImageEnhancement.dwLSEDetailLevel);
        }

        type = struImageEnhancement.byPaletteMode;

        type = (type + 1) % 23;
        struImageEnhancement.byPaletteMode = type;

        struConfigInputInfo.lpCondBuffer = &struCond;
        struConfigInputInfo.dwCondBufferSize = sizeof(struCond);


        struImageEnhancement.byNoiseReduceMode = 2; // 专家模式
        struImageEnhancement.dwGeneralLevel = 50;
        struImageEnhancement.dwFrameNoiseReduceLevel = 50;
        struImageEnhancement.dwInterFrameNoiseReduceLevel = 50;
        struImageEnhancement.byLSEDetailEnabled = 1;
        struImageEnhancement.dwLSEDetailLevel = 50;
        struConfigInputInfo.lpInBuffer = &struImageEnhancement;
        struConfigInputInfo.dwInBufferSize = sizeof(struImageEnhancement);

        if (!USB_SetDeviceConfig(m_lUserID, USB_SET_IMAGE_ENHANCEMENT, &struConfigInputInfo, NULL))
        {
            LOGIN_INFO->WriteLog(ERR_LEVEL, "FAILED  USB_GET_IMAGE_ENHANCEMENT");
            DWORD dwError = USB_GetLastError();
            LOGIN_INFO->WriteLog(INF_LEVEL, "ErrorCode = [%d], ErrorMsg = [%s]", dwError, USB_GetErrorMsg(dwError));
        }
        else
        {
            LOGIN_INFO->WriteLog(INF_LEVEL, "SUCCESS  USB_SET_IMAGE_ENHANCEMENT byPaletteMode[%d]", struImageEnhancement.byPaletteMode);
        }
        break;
```
