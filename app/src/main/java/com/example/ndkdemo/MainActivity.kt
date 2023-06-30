package com.example.ndkdemo

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.ndkdemo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private var text = ""


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        setStringForText()
        Log.e("MainActivity", " text= $text")
        // Example of a call to a native method
        binding.sampleText.text = text
        //onCreate中调用callJavaCallBack方法，在callJavaCallBack里面通过Native访问Java层方法callBack修改文本的值
        callJavaCallBack()
        val user: User = setInfoForUser(User("",1))
        Log.e("MainActivity", "setInfoForUser= $user")
        val userFromNative = getNewUser()
        Log.e("MainActivity", "userFromNative= $userFromNative")
        //测试普通引用
       // testReference()
        //测试全局引用
       // testGlobalReference()
        //测试弱引用
       // testWeakReference()
        // 测试native传过来的数组
        val intArray=newIntArrayInNative()
        Log.e("MainActivity=", "intArray =${intArray.contentToString()}")
        // 测试java传到native修改后的数组
        val array=changeArrayByCopy(intArrayOf(1,2,3,4,5,6,7))
        Log.e("MainActivity=", "array =${array.contentToString()}")
        // 测试通过数组指针修改Java数组
        val sum=getArraySumByPointer(intArrayOf(1, 2, 3, 4, 5))
        Log.e("MainActivity=", "sum =${sum}")
        //动态注册
        dynamicNative()
        dynamicNative(11)
        //jni创建线程
        createNativeThread()
        Log.e("MainActivity","main Thread:" + Thread.currentThread());
    }
    // Java和Native层传参和获取Native返回值
    /**
     * A native method that is implemented by the 'ndkdemo' native library,
     * which is packaged with this application.
     * 返回值String接受Native的传参
     */
    external fun stringFromJNI(): String?

    /**
     * Java传字符串给Native
     */
    external fun stringForJNI(s: String?): String?

    //Native访问Java层属性和方法
    //定义Native方法给text赋值
    //open 相当于java的public   external相当于java的native
    open external fun setStringForText()

    //修改tv的值，准备给Native层调用，让Native修改tv显示的字符串
     open fun callBack(s: String) {
        binding.sampleText.text=s
    }

    open external fun callJavaCallBack();


    // 对于基本类型来说，我们不需要手动转为jint或者jboolean，直接返回或者传参即可，比如
    external fun intFromJNI(): Int

    external fun booleanFromJNI(): Boolean

    /**
     * 数据类型
     */
    external fun dataType(num:Int,b:Boolean,str:String,c:Char,byte:Byte,intArray: IntArray,byteArray: ByteArray,booleanArray: BooleanArray,arrayObjects:Array<Any>)

    /**
     * Native中修改Java传过来的对象
     */
    external fun setInfoForUser(user: User?): User

    /**
     * Native创造一个Java对象并返回给Java层
     */
    external fun getNewUser(): User

    /**
     * 普通引用
     */
    external fun testReference()

    /**
     * 全局引用
     */
    external fun testGlobalReference():User
    /**
     * 弱引用
     */
    external fun testWeakReference():User

    /**
     * 处理数组：Native层创建数组再返回给Java
     */
    external fun newIntArrayInNative():IntArray
    /**
     * 处理数组：修改Java传进来的数组(1、通过操作数组副本修改java数组 2、通过数组指针修改Java数组)
     */
    external fun changeArrayByCopy(intArray: IntArray):IntArray
    /**
     * 处理数组：修改Java传进来的数组(2、通过数组指针修改Java数组)
     */
    external fun getArraySumByPointer(intArray: IntArray):Int

    /**
       方法动态注册
       之前讲的Native都是Java_ + 类A全名 +方法b拼接而成的名字去和Java对应的Native方法做映射的，这里的缺点也是很明显的，就是一旦加载动态链接库的Java类修改名字，那Native的方法也得全部修改，于是除了这一种，还有另一种应对修改更加得心应手的方法注册方式：动态注册。
        何为动态注册，就是在执行Native代码的某个时刻手动注册这些方法，这里jni选择的时刻就是当动态链接库加载的时候（即Java中调用System.loadLibrary的时候）。
        这里步骤分为3步：
        创建好Java的Native方法和对应的C++层方法，注意现在C++方法的名字可以自由发挥。
        在C++中创建一个JNINativeMethod数组，指明C++方法和Java方法的映射关系。
        在C++的JNI_OnLoad方法中完成对Native方法的注册。(JNI_OnLoad函数是当动态链接库被加载时回调的一个函数，该函数返回当前动态链接库需要的jni版本给jvm，一般可以做一些初始化的工作，比如方法的注册。)
     */
    open external fun dynamicNative()

    external fun dynamicNative(i: Int): String?

    // jni创建线程
    //触发C++创建线程的方法
    external fun createNativeThread()

    //C++创建的线程回调的Java方法
    private fun callBackForNewThread(){
        Log.e("MainActivity","callBackForNewThread Thread:" + Thread.currentThread());
    }

    companion object {
        // Used to load the 'ndkdemo' library on application startup.
        init {
            System.loadLibrary("ndkdemo")
        }
    }
}