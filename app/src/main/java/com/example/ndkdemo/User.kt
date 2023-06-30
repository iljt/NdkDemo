package com.example.ndkdemo

/**
  https://blog.csdn.net/c6E5UlI1N/article/details/111601926
  kotlin中 不管是class 还是data class 都不会自动帮你生成 无参的构造函数。
有没有方法可以避免这种情况。当然是可以的。
比如我们把定义的属性 手动给他指定一个默认值
class KUser(var name:String="")
data class KUser2(var name: String="")

或者手动指定一个无参的构造函数
data class KUser2(var name:String ,var age:Int){
constructor():this("123",0)
}
总之只要你指定了全部属性 都有默认值 ，那么就肯定会有无参的构造函数的
 */
data class User(var name:String="",var age:Int=0)