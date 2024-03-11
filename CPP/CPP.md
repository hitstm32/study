# 15.面向对象程序设计

## 15.2 定义基类和派生类

```cpp
class Quote {  
public:  
    Quote() = default;  
    Quote(const std::string& book, double sales_price) : bookNo(book), price(sales_price) {}  
    std::string isbn() const { return bookNo; }  
    virtual double net_price(std::size_t n) const { return n * price; }  
​  
    virtual ~Quote() = default;  
private:  
    std::string bookNo;  
protected://子类中可以访问  
    double price = 0.0;  
};
```

派生类：

```cpp
class Bulk_quote :public Quote {  
public:  
    Bulk_quote() = default;  
    Bulk_quote(const std::string&, double, std::size_t, double);  
​  
    double net_price(std::size_t) const override;  
​  
private:  
    std::size_t min_qty = 0;  
    double discount = 0.0;  
};
```

派生类包含基类，所以基类的指针可以指向派生类中基类的部分： ![[Pasted image 20240224012117.png]]

### 派生类构造函数：

![[Pasted image 20240224012223.png]] 通过初始化列表调用基类初始化函数，初始化基类成员。 基类中的静态成员被基类和所有派生类共享。

## 15.3虚函数