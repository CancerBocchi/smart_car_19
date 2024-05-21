# RT-Thread学习

## 第一章 

### 1.1 RTOS简介

RTOS相比于裸机以及前后台的编程方式而言，最大的优点是任务之间是解耦的，没有相互干扰，无论是前后台还是裸机程序，都必须执行完一个任务后在执行另一个任务，但是RTOS中，两个任务的执行是近似同时执行的，之间没有任何的先后顺序

想要实现这样的功能，我需要了解到线程的概念，每个线程对应一个任务，在主函数中创建线程，然后启动调度器，就饿能够分别运行每个线程中的程序

```c
void task1()
{
    while(1){
        //task1 code
    }
}

void task2()
{
    while(1){
        //task2 code
    }
}

int main()
{
    //创建线程
	task_create(task1);
    task_create(task2);
   	//启动调度器
    start_scheduler();
    while(1){
        sleep();
    }
}
```

实现这样的功能，我切换线程的时候，需要保存每个线程停止运行前的所有细节

- 计算的中间值需要保存吗？需要保存
- 程序运行的位置需要保存嘛？需要保存，程序运行的位置是一个在CPU中的寄存器，名为PC
- 全局变量和局部变量无需保存，因为函数和所有程序未结束，全局变量和局部变量没有释放
- 总结：**CPU中的寄存器状态**需要保存，保存在对应线程的栈中

### 1.2 ARM汇编

#### 	对于ARM架构

- CPU对于内存（RAM）只有两个功能，读与写
- 所有的计算在CPU内部实现
- 对于 a = a + b 这样的功能，我们需要四个步骤来实现：读取变量a，读取变量b，计算a+b，写入a

#### 	ARM的CPU内部的寄存器

- LR：链接寄存器，使用跳转指令时，会自动将程序存储到该寄存器中

- SP：栈寄存器，指向的空栈的最高位地址

- PC：程序计数器

- PSC：程序状态寄存器，保存比较指令的结构以及中断使能状态

  > [!NOTE]
  >
  > 栈的概念：栈指的是只能在一段插入或者删除的线性表；栈的最大容量时确定的，sp指向的的是栈中剩余空间的最高位，每一个栈都对应一个sp寄存器

#### 	ARM的汇编指令：

- LDR R1, [a] ：将a中的数据搬运到CPU的R1寄存器中（Load加载），读取的长度根据后缀决定，B表示1个字节，H表示2个字节，默认和R为4字节
- STR R1 , [a]：将R1中的数据写入a对于的地址的RAM中（Store存储）
- ADD R0,R1,R0：将R1和R0中的值相加，并且存储到R0中（SUB是相减的意思）
- POP {R3,LR}（出），PUSH {r3,pc}（入）：入栈出栈指令，本质上是LDR和STR指令，这两条指令的地址从sp寄存器中读出；当数据入栈时，sp寄存器先-4；当数据出栈时，sp寄存器先+4；POP PUSH指令相当于多次调用LDR和STR并且会调整sp寄存器的值
- BL  A（Branch and link）：记录下A函数语句的地址并且跳转到A函数的内部进行运行，当A函数执行结束后，跳转回A函数语句的地址并且执行A函数语句的下一条语句
- MOV R1,R3：将R3的值复制到R1中去，CPU内部的赋值

## 第二章 线程

### 2.1 保护切换上下文

当线程进行切换时，这个时候，程序运行到了某一个线程的某个语句，我们切换的步骤应该是，将这是CPU寄存器的状态进行保存入栈，然后运行另一线程，之后回到该线程时，将CPU之前的寄存器信息出栈，继续运行程序

保存代码时，相当于将所有寄存器入栈，SP寄存器会自动偏移，以至于能够存储下CPU所有寄存器的值

### 2.2 线程创建理论

rt-thread 创建线程的方法

```C
/**
 * This function will initialize a thread, normally it's used to initialize a
 * static thread object.
 *
 * @param thread the static thread object 
 * @param name the name of thread, which shall be unique 
 * @param entry the entry function of thread
 * @param parameter the parameter of thread enter function
 * @param stack_start the start address of thread stack
 * @param stack_size the size of thread stack
 * @param priority the priority of thread
 * @param tick the time slice if there are same priority thread
 *
 * @return the operation status, RT_EOK on OK, -RT_ERROR on error
 */
//静态创建
rt_err_t rt_thread_init(struct rt_thread *thread,		//线程句柄
                        const char       *name,			//线程名字
                        void (*entry)(void *parameter),	//入口函数
                        void             *parameter,	//入口函数参数
                        void             *stack_start,	//栈地址
                        rt_uint32_t       stack_size,	//栈大小
                        rt_uint8_t        priority,		//优先级
                        rt_uint32_t       tick)			//线程时间片大小
    
/**
 * This function will create a thread object and allocate thread object memory
 * and stack.
 *
 * @param name the name of thread, which shall be unique
 * @param entry the entry function of thread
 * @param parameter the parameter of thread enter function
 * @param stack_size the size of thread stack
 * @param priority the priority of thread
 * @param tick the time slice if there are same priority thread
 *
 * @return the created thread object
 */
//动态创建 指的是线程控制块和栈的动态创建
rt_thread_t rt_thread_create(const char *name,				//线程名字
                             void (*entry)(void *parameter),//入口函数地址
                             void       *parameter,			//函数参数
                             rt_uint32_t stack_size,		//栈大小
                             rt_uint8_t  priority,			//优先级
                             rt_uint32_t tick)				//时间片大小
```

无论何种操作系统一个线程都包含以下几点

- 分配函数入口
- 分配线程控制块，比如这里的 `struct rt_thread`
- 栈：栈的地址保存在线程控制块中 
- 构造栈内容：创建线程时，栈中应当保留程序停留在函数入口时的CPU寄存器内容

我们着重来分析以下一个线程控制块中应该有哪些信息

- 函数入口地址
- 栈的相关信息：栈的大小，起始与当前地址

在RT_Thread中，结构体内容如下

```c
struct rt_thread
{
    /* rt object */
    char        name[RT_NAME_MAX];                      /**< the name of thread */
    rt_uint8_t  type;                                   /**< type of object */
    rt_uint8_t  flags;                                  /**< thread's flags */

#ifdef RT_USING_MODULE
    void       *module_id;                              /**< id of application module */
#endif

    rt_list_t   list;                                   /**< the object list */
    rt_list_t   tlist;                                  /**< the thread list */

    /* stack point and entry */
    void       *sp;                                     /**< stack point */
    void       *entry;                                  /**< entry */
    void       *parameter;                              /**< parameter */
    void       *stack_addr;                             /**< stack address */
    rt_uint32_t stack_size;                             /**< stack size */

    /* error code */
    rt_err_t    error;                                  /**< error code */

    rt_uint8_t  stat;                                   /**< thread status */

#ifdef RT_USING_SMP
    rt_uint8_t  bind_cpu;                               /**< thread is bind to cpu */
    rt_uint8_t  oncpu;                                  /**< process on cpu` */

    rt_uint16_t scheduler_lock_nest;                    /**< scheduler lock count */
    rt_uint16_t cpus_lock_nest;                         /**< cpus lock count */
    rt_uint16_t critical_lock_nest;                     /**< critical lock count */
#endif /*RT_USING_SMP*/

    /* priority */
    rt_uint8_t  current_priority;                       /**< current priority */
    rt_uint8_t  init_priority;                          /**< initialized priority */
#if RT_THREAD_PRIORITY_MAX > 32
    rt_uint8_t  number;
    rt_uint8_t  high_mask;
#endif
    rt_uint32_t number_mask;

#if defined(RT_USING_EVENT)
    /* thread event */
    rt_uint32_t event_set;
    rt_uint8_t  event_info;
#endif

#if defined(RT_USING_SIGNALS)
    rt_sigset_t     sig_pending;                        /**< the pending signals */
    rt_sigset_t     sig_mask;                           /**< the mask bits of signal */

#ifndef RT_USING_SMP
    void            *sig_ret;                           /**< the return stack pointer from signal */
#endif
    rt_sighandler_t *sig_vectors;                       /**< vectors of signal handler */
    void            *si_list;                           /**< the signal infor list */
#endif

    rt_ubase_t  init_tick;                              /**< thread's initialized tick */
    rt_ubase_t  remaining_tick;                         /**< remaining tick */

    struct rt_timer thread_timer;                       /**< built-in thread timer */

    void (*cleanup)(struct rt_thread *tid);             /**< cleanup function when thread exit */

    /* light weight process if present */
#ifdef RT_USING_LWP
    void        *lwp;
#endif

    rt_ubase_t user_data;                             /**< private user data beyond this thread */
};
```

其中需要注意的变量如下

![image-20240221221427953](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240221221427953.png)

从创建线程的源码看，创建线程的过程与我们的分析一致，只是有些细节需要主义

```c
static rt_err_t _rt_thread_init(struct rt_thread *thread,
                                const char       *name,
                                void (*entry)(void *parameter),
                                void             *parameter,
                                void             *stack_start,
                                rt_uint32_t       stack_size,
                                rt_uint8_t        priority,
                                rt_uint32_t       tick)
{
    /* init thread list */
    rt_list_init(&(thread->tlist));

    thread->entry = (void *)entry;
    thread->parameter = parameter;

    /* stack init */
    thread->stack_addr = stack_start;
    thread->stack_size = stack_size;

    /* init thread stack */
    rt_memset(thread->stack_addr, '#', thread->stack_size);
#ifdef ARCH_CPU_STACK_GROWS_UPWARD
    thread->sp = (void *)rt_hw_stack_init(thread->entry, thread->parameter,
                                          (void *)((char *)thread->stack_addr),
                                          (void *)rt_thread_exit);
#else
    thread->sp = (void *)rt_hw_stack_init(thread->entry, thread->parameter,
                                          (rt_uint8_t *)((char *)thread->stack_addr + thread->stack_size - sizeof(rt_ubase_t)),
                                          (void *)rt_thread_exit);
#endif

    /* priority init */
    RT_ASSERT(priority < RT_THREAD_PRIORITY_MAX);
    thread->init_priority    = priority;
    thread->current_priority = priority;

    thread->number_mask = 0;
#if RT_THREAD_PRIORITY_MAX > 32
    thread->number = 0;
    thread->high_mask = 0;
#endif

    /* tick init */
    thread->init_tick      = tick;
    thread->remaining_tick = tick;

	/* 省略 code... */

    return RT_EOK;
}
```

可以看出，创建线程代码主要是对线程控制结构体进行了赋值，其中我们比较关心sp赋值的函数，因为sp的赋值关系到栈的构造

```c
rt_uint8_t *rt_hw_stack_init(void       *tentry,
                             void       *parameter,
                             rt_uint8_t *stack_addr,
                             void       *texit)
{
    struct stack_frame *stack_frame;
    rt_uint8_t         *stk;
    unsigned long       i;
	//计算stk即构造栈后计算sp的值
    stk  = stack_addr + sizeof(rt_uint32_t);
    stk  = (rt_uint8_t *)RT_ALIGN_DOWN((rt_uint32_t)stk, 8);
    stk -= sizeof(struct stack_frame);
	//结构体入栈，为结构体提供了存储的空间
    stack_frame = (struct stack_frame *)stk;

    /* init all register */
    for (i = 0; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i ++)
    {
        ((rt_uint32_t *)stack_frame)[i] = 0xdeadbeef;
    }

    stack_frame->exception_stack_frame.r0  = (unsigned long)parameter; /* r0 : argument */
    stack_frame->exception_stack_frame.r1  = 0;                        /* r1 */
    stack_frame->exception_stack_frame.r2  = 0;                        /* r2 */
    stack_frame->exception_stack_frame.r3  = 0;                        /* r3 */
    stack_frame->exception_stack_frame.r12 = 0;                        /* r12 */
    stack_frame->exception_stack_frame.lr  = (unsigned long)texit;     /* lr */
    stack_frame->exception_stack_frame.pc  = (unsigned long)tentry;    /* entry point, pc */
    stack_frame->exception_stack_frame.psr = 0x01000000L;              /* PSR */

#if USE_FPU
    stack_frame->flag = 0;
#endif /* USE_FPU */

    /* return task's current stack address */
    return stk;
}
```

我们发现，sp实际上对应了函数中的stk变量，根据stk的计算过程，我们又可以注意到stack_frame结构体

```c
struct stack_frame
{
#if USE_FPU
    rt_uint32_t flag;
#endif /* USE_FPU */

    /* r4 ~ r11 register */
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t r11;

    struct exception_stack_frame exception_stack_frame;
};

struct exception_stack_frame
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r12;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t psr;
};
```

我们可以发现，该结构体包含的均为内核内部的寄存器，这样我们就可以知道该函数实际上时对栈进行了一个构造，构造时是根据结构体的大小将整个结构体入栈

> [!NOTE]
>
> ## 总结：
>
> RT-thread创建线程的过程为：
>
> - 将线程初始的重要参数赋予线程控制结构体
> - 根据初始参数，计算sp寄存器的值并且构造栈的内容
> - 创建函数 `rt_thread_create()` `rt_thread_init()`
> - 控制结构体 `rt_thread` 
> - 栈初始化结构体 `stack_frame`

### 2.3 线程调度

RT-Thread 的调度策略

- 可抢占：高优先级先执行
- 轮转：相同优先级轮转执行

#### 2.3.1 RT-Thread 的任务调用机制1：链表

- RT-Thread使用链表来调度任务
- 每个优先级都对应一个首位相连的双向链表
- 先判断高优先级的任务是否被启用，若启用则运行高优先级，反之运行低优先级
- 每个链表中，总是运行链表头，一个任务呗运行完毕后，会将该任务移至链表尾部，剩下的任务往前填充

![A0BE9644203F55AA8E6EADA5D3B7AB69](C:\Users\Administrator\Downloads/A0BE9644203F55AA8E6EADA5D3B7AB69.png)

源码分析：

```c
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];//管理优先级

//链表结构体
struct rt_list_node
{
    struct rt_list_node *next;                          /**< point to next node. */
    struct rt_list_node *prev;                          /**< point to prev node. */
};
typedef struct rt_list_node rt_list_t;

//将n链表插入到l链表前面 当线程切换时，会调用该函数
rt_inline void rt_list_insert_before(rt_list_t *l, rt_list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

//该函数只是将线程插入链表，并不是开始运行
rt_err_t rt_thread_startup(rt_thread_t thread);-->rt_thread_resume(thread);
-->
rt_list_insert_before(rt_list_t *l, rt_list_t *n);-->rt_schedule_insert_thread(thread);
-->
rt_list_insert_before();
//通过细节可知，最后调用的插入函数，是将线程插入到优先级链表的末尾位置

//该函数时启动整个系统
int rtthread_startup(void);
-->//调用以下函数 即打开系统调度器
void rt_system_scheduler_start(void) 
```

#### 2.3.2 RT-Thread的内部机制2--定时器

我们已经知道RT-Thread时如何管理线程调度的，但是RT-Thread时如何知道某个线程运行结束的呢？这就要引入第二个机制，定时器

RT-Thread会调用系统定时器中断，在系统定时器中断中判断每个Thread是否运行结束

- 结束-->切换线程
- 未结束-->中断返回

代码分析：我们首先找到系统中断 `void SysTick_Handler(void)` 发现移植好的工程中有以下语句

```c
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();//用于切换线程的函数
    sdmmc_tick_handler();
    /* leave interrupt */
    rt_interrupt_leave();
}

void rt_tick_increase(void)
{
    struct rt_thread *thread;

    /* increase the global tick */
#ifdef RT_USING_SMP
    rt_cpu_self()->tick ++;
#else
    ++ rt_tick;
#endif

    /* check time slice */
    thread = rt_thread_self();

    -- thread->remaining_tick;
    if (thread->remaining_tick == 0)//若线程的tick归零 则切换线程
    {
        /* change to initialized tick */
        thread->remaining_tick = thread->init_tick;

        thread->stat |= RT_THREAD_STAT_YIELD;//将状态置位

        rt_schedule();//切换线程
    }

    /* check timer */
    rt_timer_check();
}
```

