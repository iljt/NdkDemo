//jni.h是实现jni的基本头文件，大量的jni方法和变量都定义在里面，所以是必须引入的头文件。
#include <jni.h>
#include <string>
//线程导包
#include <pthread.h>
#include "logger.h"
//我们写的是一个C++程序，C++支持方法重载，为了能够区分重载的方法，C++程序在编译的时候会将参数信息和方法名结合起来形成一个新的方法名。
//“extern "C"”就是告诉编译器，该方法是C语言的方法，让编译器不用将参数信息和方法名结合起来形成一个新的方法名，使用原来的方法即可。所以如果没有添加“extern "C"”，则该方法就会按照C++的方式将参数信息和方法名结合起来形成一个新的方法名，这样在运行过程中，当Java调用Native方法的时候，会按照Java_ + 类全名 +Native方法的方式去搜索对应的C++方法，这样子就会找不到而报错。
//JNIEXPORT 确定一个方法是否放入动态链接库的一个动态方法表里，该方法表可以看做是给jni提供可以让Java层调用的方法，为什么这么做呢，因为这样可以控制Native方法的暴露，增加反编译难度 https://stackoverflow.com/questions/19422660/when-to-use-jniexport-and-jnicall-in-android-ndk
//jstring 方法的返回值
//JNICALL 是jni.h一个空的宏定义，那有啥用？ JNICALL这个是一种说明标识该方法可以应用于jni的标识，不过在android中，这个标识为空，但是在其他平台，比如Windows中，JNICALL的的值为：“__stdcall”
extern "C" JNIEXPORT jstring JNICALL

//JNIEnv* env在jn是一个核心的参数，我们说jni是Java和C/C++的桥梁，那么支撑这个桥梁的钢筋混泥土。
//JNIEnv是一个线程相关的结构体, 该结构体代表了 Java 在本线程的运行环境。通过JNIEnv可以调用到一系列JNI系统函数。
//每个线程中都有一个 JNIEnv 指针。JNIEnv只在其所在线程有效, 它不能在线程之间进行传递。

//jobject 如果一个Native函数在Java中声明为成员方法，则一定会带上这个参数，他代表的是调用该方法的Java对象，为Native访问Java对象提供了通道。

//可以把Java和Native代码想象为2个平行宇宙，那么一个在Java宇宙的类型为String的东西，在Native宇宙中它就不叫做String了，它变成了jstring的东西，同理，在Java宇宙的类型为Object的东西，在Native宇宙中就变成了jobject。除了这两个以外，当然还有许多的这样的映射关系
Java_com_example_ndkdemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    //创建一个C++字符串
    std::string hello = "Hello from C++";
    //将C++字符串转为jstring 再返回给Java层。hello.c_str将string转为char*类型。
    //首先创建一个C++字符串hello ，因为Java的和Native是2个平行宇宙，所以string 是不能直接给Java的，所以需要通过NewStringUTF转为jstring才能安心交给Java层处理。
    //可以看到，NewStringUTF方法就是由JNIEnv类型的env调用的，之前说JNIEnv是为Java和Native提供交互的桥梁，在这里就可以看出具体就是通过JNIEnv可以将Native层的数据类型转为jni类型，再交给Java层。
    //native向java层传参数
    return env->NewStringUTF(hello.c_str());
}

/**
 * 通过Java传入的字符串会转化为jstring形式传给Native,这里C++必须通过GetStringUTFChars方法转化为C++中的字符串，才能被C++处理。
  这里要注意的是，一般情况下，Java层传入的jstring如果确定在C++层不再使用了，需要通过ReleaseStringUTFChars方法释放对Java层对象的持有防止内存泄漏：
 */
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndkdemo_MainActivity_stringForJNI(JNIEnv *env, jobject thiz, jstring s) {
    jboolean isCopy = JNI_FALSE;
    //isCopy是JVM返回的标志位。返回JNI_TRUE表示原字符串的拷贝。返回JNI_FALSE表示返回原字符串的指针，即指向原来Java的字符串
    const char* c = env->GetStringUTFChars(s, &isCopy);
    //这里为了简单就直接原封不动返回给Java
    jstring result = env->NewStringUTF(c);
    //字符串c不再使用到则释放对Java层字符串的引用
    env->ReleaseStringUTFChars(s,c);
    return result;
}

// jni数据类型
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_dataType(JNIEnv *env, jobject thiz, jint num, jboolean b,
                                               jstring str, jchar c, jbyte byte,
                                               jintArray int_array, jbyteArray byte_array,
                                               jbooleanArray boolean_array,
                                               jobjectArray array_objects) {
    //找到类的class 2种方式
    //直接用GetObjectClass找到Class, 也就是MainActivity.class.
    jclass  jMainActivityClass= env ->GetObjectClass(thiz);
    if(jMainActivityClass == 0){
        LOGE("find class error","Unable to find class");
        return;
    }
    //直接用findClass找到Class, 也就是Sdk.class.
    jclass jMainActivityClass2 =env->FindClass("com/example/ndkdemo/MainActivity");
    if (jMainActivityClass2 == 0) {
        LOGE("find class error","Unable to find class");
        return;
    }

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_ndkdemo_MainActivity_intFromJNI(JNIEnv *env, jobject thiz) {
    //对于基本类型来说，我们不需要手动转为jint或者jboolean，直接返回或者传参即可
    return 1;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_ndkdemo_MainActivity_booleanFromJNI(JNIEnv *env, jobject thiz) {
    //对于基本类型来说，我们不需要手动转为jint或者jboolean，直接返回或者传参即可
    return false;
}


/**
 * 这里参数jobject就是如果一个Native函数在Java中声明为成员方法，则一定会带上这个参数，他代表的是调用该方法的Java对象，那么在这里，它就是MainActivity在Native的映射，这里操作分为3步：
    通过jobject参数获取MainActivity对应的class对象在Native层的映射jclass 对象。
    通过jclass 对象得到text属性的id
    通过text属性id去修改text的值
    其实这就是一个反射的操作，这里要注意的一点就是第2步得到text属性的id的方法GetFieldID第最后一个参数，它代表访问的属性的类型描述符。
    在JVM虚拟机中，存储数据类型的名称时，是使用指定的描述符(签名)来存储，而不是我们习惯的 int，float 对照表如下：
    Java 类型	符号
    boolean	Z
    byte	B
    char	C
    short	S
    int	    I
    long	L
    float	F
    double	D
    void	V
    objects对象	Lfully-qualified-class-name;L全类名 如：String -> Ljava/lang/String;
    Arrays数组	[array-type [数组类型 如：int[] ->  [I
    methods方法	(argument-types)return-type(参数类型)返回类型 如：int add(int a,int b) ->  (II)I  或者 void add(int a,int b) ->  (II)V
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_setStringForText(JNIEnv *env, jobject thiz) {
    //Native访问Java层属性
    // implement java setStringForText()
    //获取对象的Class
    jclass  jMainActivityClass =env->GetObjectClass(thiz);
    //获取java层的text属性
   jfieldID  textfieldId = env->GetFieldID(jMainActivityClass,"text","Ljava/lang/String;");
    //给text属性赋值
   jstring  textvalue =env->NewStringUTF("我是从native 设置的值");
   env->SetObjectField(thiz,textfieldId,textvalue);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_callJavaCallBack(JNIEnv *env, jobject thiz) {
    //  implement callJavaCallBack()
    //Native访问Java层方法
    //获取jclass对象
    jclass jMainActivityClass = env->GetObjectClass(thiz);
    //获取Java方法的jmethodID
    jmethodID methodId =env->GetMethodID(jMainActivityClass,"callBack","(Ljava/lang/String;)V");
    std::string  str ="I am CallBack From C++";
    jstring  jstr = env->NewStringUTF(str.c_str());
    //调用Java方法，传参必须是jni类型
    env->CallVoidMethod(thiz,methodId,jstr);
}

/**
 * 要对Java对象的属性进行修改依然是反射操作，首先拿到属性的id，然后调用JNIEnv 的set(类型名)Field就可以修改。
 * 还是之前说过的3步曲：
    获取Java对象对应的class对象在Native层的映射jclass 对象。
    通过jclass 对象得到属性的id。
    通过属性id去修改值。
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_ndkdemo_MainActivity_setInfoForUser(JNIEnv *env, jobject thiz, jobject user) {
    //  implement setInfoForUser()
    // 找到对象的Java类
    jclass  userClass = env->FindClass("com/example/ndkdemo/User");
    // 对应的Java属性
    jfieldID name = env->GetFieldID(userClass,"name","Ljava/lang/String;");
    jfieldID age = env->GetFieldID(userClass,"age","I");
    //给传过来的user对象赋值
    env->SetObjectField(user,name,env->NewStringUTF("我是张三，我被native修改了"));
    env->SetIntField(user,age,20);
    return user;
}

/**
 * 和上面方法一个套路，唯一不同的就是为了创建新对象，要先获得User的无参构造方法id，然后通过构造方法id创建User对象，接着又是上面说的三部曲修改User对象属性值。
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_ndkdemo_MainActivity_getNewUser(JNIEnv *env, jobject thiz) {
    // implement getNewUser()
    jclass userClass = env->FindClass("com/example/ndkdemo/User");
    // 获取类的构造函数，记住这里是调用无参的构造函数
    jmethodID id = env->GetMethodID(userClass, "<init>", "()V");
    // 创建一个新的对象
    //在jni中，无论是从Java层作为方法参数传给Native层或者是Native层方法中创建的Java对象（jobject等jni对象也属于），
    // 默认为被局部引用指向，即会在方法返回的时候自动释放引用，也可以在方法返回前手动释放。和Java一样，释放引用的目的是告诉jvm这个对象这边已经不再需要使用，让jvm决定是否回收。
    jobject  user_=env->NewObject(userClass,id);
    //局部引用
    //也可以显式指定为局部引用
   // user_ =  env->NewLocalRef(user_);
   // env->DeleteLocalRef(user_);
   //后面不再使用user_对象 这里user_ 是在Native中创建的jni对象，本质是指向jvm堆中的user_对象的局部引用，一旦引用被释放，
   // 后面不能再使用user_这个引用。（闪退~）

    // 对应的Java属性
    jfieldID name = env->GetFieldID(userClass,"name","Ljava/lang/String;");
    jfieldID age = env->GetFieldID(userClass,"age","I");
    //给传过来的user对象赋值
    env->SetObjectField(user_,name,env->NewStringUTF("我是native新创建的User对象"));
    env->SetIntField(user_,age,33);
    return user_;
}

/**
 * 局部引用在Native方法执行完就会自动释放，那假如我们需要一个能跨方法使用，或者跨线程使用的对象呢？当然有局部，就会有全局引用，如果按照Java习惯的写法，就会这样写：
 */
//定义一个“全局引用”,其实在这里和Java不一样，这里只是普通的引用
jclass userClass;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_testReference(JNIEnv *env, jobject thiz) {
    // implement testReference()
    //第二次进入虽然personClass不为null，但是指向的对象已经被释放了，成为悬空指针
    if (userClass == nullptr){
        userClass = env->FindClass("com/example/ndkdemo/User");
    }
    LOGD("userClass=","testReference GetMethodID");
    // 获取类的构造函数，记住这里是调用无参的构造函数
    jmethodID id = env->GetMethodID(userClass, "<init>", "()V");
    //这样只有第一次调用testReference方法的时候创建了personClass 对象，以后调用testReference方法不再创建，运行下，闪退。
    //为什么呢？因为在Java这样写，jvm不会看到userClass 引用一直指向堆中对应对象，jvm不会回收。
    // 但是这里用一个普通的引用引用该对象，那不好意思，一旦方法执行完，jvm就回收该对象，那么personClass这个引用在第二次执行testReference方法的时候确实不为null，但是它已经成为一个悬空指针。
    //所以这里要使用全局引用才是正道
}

//定义一个全局引用
//全局引用相当于告诉jvm，这个对象我还要用，你千万不要释放，所以在手动释放之前，jvm不会将该对象释放，所以要切记要在不需要使用该对象的时候手动释放该全局引用
jobject userGlobalReference;

extern "C"
JNIEXPORT jobject  JNICALL
Java_com_example_ndkdemo_MainActivity_testGlobalReference(JNIEnv *env, jobject thiz) {
    // implement testGlobalReference()
    jclass userClass1 = nullptr;
    if (userGlobalReference == nullptr){
        userClass1 = env->FindClass("com/example/ndkdemo/User");
        // 获取类的构造函数，记住这里是调用无参的构造函数
        jmethodID id = env->GetMethodID(userClass1, "<init>", "()V");
        // 创建一个新的对象
        jobject user = env->NewObject(userClass1, id);
        //关键语句，将person 包装为全局引用再赋给personGlobalReference
        userGlobalReference = env->NewGlobalRef(user);
    }

    jfieldID name = env->GetFieldID(userClass1, "name", "Ljava/lang/String;");
    jfieldID age = env->GetFieldID(userClass1, "age", "I");

    env->SetObjectField(userGlobalReference, name, env->NewStringUTF("global Reference 啊啊啊"));
    env->SetIntField(userGlobalReference, age, 20);
    //同样，一旦释放，就不能再次使用该引用
   // env->DeleteGlobalRef(userGlobalReference);
    return userGlobalReference;

}

/**
 * Java为了防止内存泄漏，经常在内部类或者异步等情况下会使用弱引用，那么Native层同样也有这样的需求，又想要有全局引用的效果，又不想因此组织jvm对对象的回收。
 * 和Java一样，和全局引用很相似，只是需要再多判断一步弱引用指向的对象是否已经被回收即可
 */
//定义一个弱引用
jobject userWeakReference;

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_ndkdemo_MainActivity_testWeakReference(JNIEnv *env, jobject thiz) {
    //  implement testWeakReference()
    //判断弱引用指向的对象是否已经被回收
    jclass userClass= nullptr;

    jboolean isEqual = env->IsSameObject(userWeakReference,NULL);
   if(userWeakReference == nullptr || isEqual){
       userClass = env->FindClass("com/example/ndkdemo/User");
       // 获取类的构造函数，记住这里是调用无参的构造函数
       jmethodID id = env->GetMethodID(userClass, "<init>", "()V");
       // 创建一个新的对象
       jobject user = env->NewObject(userClass, id);
        //关键语句，将user 包装为弱引用再赋给userWeakReference
        //此时personWeakReference 引用了Java层的person对象，
        // 但是并不阻止jvm回收person对象，所以在下次使用的时候，需要听通过env->IsSameObject方法判断弱引用引用的对象是否已经被jvm回收。
       userWeakReference = env->NewWeakGlobalRef(user);
       env->DeleteLocalRef(user);
       //同样，一旦释放，就不能再次使用该引用 否则报错
      // env->DeleteWeakGlobalRef(userWeakReference);
   }
    return userWeakReference;
}

/**
 * 处理数组:Native层创建数组再返回给Java
 *
 */
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_ndkdemo_MainActivity_newIntArrayInNative(JNIEnv *env, jobject thiz) {
    //  implement newIntArrayInNative()
    //  在Native层中，jintArray 就是Java中int[]“平行世界”的映射，不过在Native中jintArray 的使用确实比较别扭，不能一步到位，
    //  要先创建好jint 数组，然后再创建jintArray 对象，最后将jint 数组通过SetIntArrayRegion方法赋值给jintArray ，然后把jintArray 返回给Java。
    jint nativeArry[5] ={1,2,3,4,5};
    jintArray intArray = env->NewIntArray(5);
    env->SetIntArrayRegion(intArray,0,5,nativeArry);
    return intArray;
}

/**
 * 修改Java传进来的数组(1、通过操作数组副本修改java数组）
 */
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_ndkdemo_MainActivity_changeArrayByCopy(JNIEnv *env, jobject thiz,
                                                        jintArray int_array) {
    //implement changeArrayByCopy()
    //获得Java传递进来数组的长度
    jsize  size = env->GetArrayLength(int_array);
    //定义一个C数组
    //将Java数组区复制到jint数组中
    jint nativeArray[size];
    env->GetIntArrayRegion(int_array,0,size,nativeArray);
   //修改元素的值
    for(int i=0;i<size;i++){
        nativeArray[i]=nativeArray[i]*2;
    }
    //从C数组向Java数组提交所做的修改。0为同步数据的起始index，size为同步数据个数
    env->SetIntArrayRegion(int_array,0,size,nativeArray);
    return int_array;
}

/**
 * 修改Java传进来的数组(2、通过数组指针修改Java数组）
 * 这种方式是先通过GetIntArrayElements得到数组的指针，这里注意第二个参数，是一个返回值参数，系统通过这个参数告诉我们返回的指针指向的数组是否是原Java数组还是原Java数组的拷贝。
这里操作数组结束之后，切记要调用ReleaseIntArrayElements释放指针nativeArray，这个方法还有一个重点，就是在GetIntArrayElements返回的isCopy是true的情况下，还能将对副本的处理同步到Java数组。
第三个参数是指明具体处理方式：
mode                                                             actions
0                                    copy back the content and free the elems buffer（拷贝副本内容到原始数组并且释放数组指针）
JNI_COMMIT                copy back the content but do not free the elems buffer（拷贝副本内容到原始数组，但不释放数组指针）
JNI_ABORT                   free the buffer without copying back the possible changes（释放数组指针，但是不拷贝副本内容到原始数组）


 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_ndkdemo_MainActivity_getArraySumByPointer(JNIEnv *env, jobject thiz,jintArray int_array) {
    // implement getArraySumByPointer()
    jboolean isCopy;
    jint sum = 0;
    //获得Java传递进来数组的长度
    jsize size=env->GetArrayLength(int_array);
     //用GetArrayElements函数获取指向数组元素的直接指针，isCopy表示指针指向的数组是否是从Java数组拷贝出来的副本
    jint* nativeArray=env->GetIntArrayElements(int_array, &isCopy);
    for (int i = 0; i < size; ++i){
        sum+=nativeArray[i];
    }
    //释放nativeArray指针，并将数据同步到Java数组如果GetIntArrayElements有发生Copy
    env->ReleaseIntArrayElements(int_array, nativeArray, 0);
    return sum;
}

/**
 * 动态注册
 */
extern "C"
JNIEXPORT void JNICALL
dynamicNative1(JNIEnv *env, jobject thiz) {
    LOGE("","dynamicNative1 动态注册");
}

extern "C"
JNIEXPORT jstring JNICALL
dynamicNative2(JNIEnv *env, jobject thiz, jint i) {
    LOGE("","dynamicNative2 动态注册");
    return env->NewStringUTF("我是动态注册的dynamicNative2方法");
}

//需要动态注册的方法数组
/**
 * JNINativeMethod是一个结构体
 *   name ：Java的Native方法名
 *   signature：Java的Native方法描述符
 *   fnPtr:C++中的函数指针
 */
static const JNINativeMethod mMethods[] = {
        {"dynamicNative","()V", (void *)dynamicNative1},
        {"dynamicNative", "(I)Ljava/lang/String;", (jstring *)dynamicNative2}
};


JavaVM* _vm = nullptr;

/**
 *  JNI_OnLoad函数是当动态链接库(即Java中调用System.loadLibrary()的时候)被加载时回调的一个函数
 *  JNIEnv对象是线程独有的，这里可以通过参数JavaVM* vm，来获取当前线程对应的JNIEnv指针，再通过JNIEnv获取加载的Java类，这里即MainActivity的jclass对象，然后通过env->RegisterNatives(mainActivityCls,mMethods,2);将方法映射数组和加载的Java类进行最终的动态注册。
 */

jint JNI_OnLoad(JavaVM* vm, void* reserved){
    //JavaVM* vm是代表虚拟机的一个对象
    _vm = vm;
    JNIEnv* env = NULL;
    //获得当前线程的JniEnv
    int r = vm->GetEnv((void**) &env, JNI_VERSION_1_4);
    if( r != JNI_OK){
        return -1;
    }
    jclass mainActivityCls = env->FindClass( "com/example/ndkdemo/MainActivity");
    // 注册 如果小于0则注册失败
    r = env->RegisterNatives(mainActivityCls,mMethods,2);
    if(r  != JNI_OK )
    {
        return -1;
    }
    return JNI_VERSION_1_4;
}


jobject instance;

void *task(void *args) {
    JNIEnv *env;
    //将本地当前线程附加到jvm，并获得jnienv
    //成功则返回0

    //将当前线程与JVM进行关联, 获取对应线程的JNIEnv 对象，拿到JNIEnv对象，就相当于拿到通往Java平行世界的钥匙，然后再按照反射方式调用Java层的callBackForNewThread方法。
    _vm->AttachCurrentThread(&env, 0);

    //获取jclass对象
    jclass activityClass = env->GetObjectClass(instance);
    //获取Java callBackForNewThread方法的jmethodID
    jmethodID callBackMethod = env->GetMethodID(activityClass, "callBackForNewThread", "()V");
    std::string hello = "I am CallBack ";
    jstring s = env->NewStringUTF(hello.c_str());
    //调用Java方法
    env->CallVoidMethod(instance, callBackMethod);
    //解除当前线程与JVM的连接 线程退出或不再需要使用JNIEnv时，我们必须通过调用DetachCurrentThread来解除连接，否则可能会导致线程不能正常退出或程序奔溃等问题
    _vm->DetachCurrentThread();
    return 0;
}

/**
 *  创建线程 pthread_create（）
    第一个参数__pthread_ptr 为指向 pthread_t 类型变量的指针，用它代表返回线程的句柄。
    第二个参数__attr 为指向 pthread_attr_t 结构的指针，可以通过该结构来指定新线程的一些属性，比如栈大小、调度优先级等，具体看 pthread_attr_t 结构的内容。如果没有特殊要求，可使用默认值，把该变量取值为 NULL 。
    第三个参数为该线程启动程序的函数指针，也就是线程启动时要执行的那个方法，类似于 Java Runnable 中的 run 方法，它的函数签名格式如下：
    void* start_routine(void* args)
    启动程序将线程参数看成 void 指针，返回 void 指针类型结果。
    第四个参数为线程启动程序的参数，也就是函数的参数，如果不需要传递参数，它可以为 NULL 。
    pthread_create 函数如果执行成功了则返回 0 ，如果返回其他错误代码。
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_createNativeThread(JNIEnv *env, jobject thiz) {
    // implement createNativeThread()
    pthread_t pid;
    //用全局引用将MainActivity实例保存起来，供子线程使用
    instance =env->NewGlobalRef(thiz);
    //创建新线程运行task方法
    pthread_create(&pid, 0, task, 0);
}




