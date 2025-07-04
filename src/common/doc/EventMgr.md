## 事件系统
+ 观察者模式
### 使用方式
  * 引用头文件common/EventMgr.h,common/EventDefine.h
  * 事件需要定义在EventDefine.h文件中，事件携带的信息作为结构体成员存在
  * 需要监听事件的类，继承EventListener接口，并实现EventListener规定的OnEvent函数
  * 在构造函数中注册事件，注册方式为：EventMgr::GetInstance().RegisterEvent(this);
  * 在析构函数中注销事件，注销方式为：EventMgr::GetInstance().UnregisterEvent(this);
  * 任意位置调用EventMgr::GetInstance().SendEvent(TEvent event),所有监听此事件的监听器都会触发OnEvent事件响应