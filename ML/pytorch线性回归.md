![[Pasted image 20240330190700.png]]
前馈->反馈->更新
使用tensor来实现线性回归，**mini-batch风格**
把loss写成矩阵形式：
![[Pasted image 20240330192111.png]]
由于采用minibatch风格，所以x_data和y_data直接用张量表示：
![[Pasted image 20240330192155.png]]
## 构造计算图
![[Pasted image 20240330192339.png]]

采用类的方式进行模型编写：
![[Pasted image 20240330193010.png]]
所有模型类都要继承Module父类：
类中最少要实现两个函数：**构造函数**和**forward函数**，如果采用这种方式构造计算图，反向传播会由框架自动实现。
构造函数中`super`是调用了父类的构造函数。
`torch.nn.Linear(1,1)`实际构建了一个线性计算块：
![[Pasted image 20240330193413.png]]
![[Pasted image 20240330193613.png]]
![[Pasted image 20240330194932.png]]
这一句实际上是python的特性，即调用一个可调用类（实际上就是该类里有个call函数，module中这个call函数里实际调用了forward，所以必须声明forward函数，以便让call调用，并完成框架中的反向传播等工作。实际上forward是覆盖掉了module里的forward函数。
## 构造损失函数和优化器：
![[Pasted image 20240330195438.png]]
`model.parameters()`就是把模型的参数`w和b`传给优化器。
![[Pasted image 20240330200003.png]]
## 训练流程：
![[Pasted image 20240330200140.png]]

![[Pasted image 20240330200425.png]]
![[Pasted image 20240330200633.png]]
# 疑问：到底怎么理解minipatch？