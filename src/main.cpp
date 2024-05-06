// #include"TcpServer.h"
// #include<iostream>
// #include<unistd.h>

// using std::cout;
// using std::endl;

// void onConnection(const TcpConnectionPtr &con){
//     cout<<con->toString()<<" has connected!"<<endl;
// }

// void onMessage(const TcpConnectionPtr &con){
//     string msg = con->receive();
//     cout<<"recv msg "<<msg<<endl;
//     //接收与发送之间，消息msg是没有做任何处理的
//     //处理msg的业务逻辑全部在此处实现的话
//     //将msg这些信息打个包交给MyTask，然后将MyTask注册给线程池
//     //让线程池去处理具体的业务逻辑，此时业务逻辑的处理就不在
//     //EventLoop线程中
//     /* MyTask task(msg); */
//     /* threadPool.addTask(task); */
//     msg = "server back:" + msg;
//     con->send(msg);
// }

// void onClose(const TcpConnectionPtr &con){
//     cout<<con->toString()<<" has closed!"<<endl;
// }

// void test()
// {
//     Acceptor acceptor("127.0.0.1", 8888);
//     acceptor.ready();//此时处于监听状态

//     EventLoop loop(acceptor);
//     //回调函数的注册
//     loop.setConnectionCallback(std::move(onConnection));
//     loop.setMessageCallback(std::move(onMessage));
//     loop.setCloseCallback(std::move(onClose));

//     loop.loop();
// }


// void test2(){
//     TcpServer server("127.0.0.1",8888);
//     server.setAllCallback(std::move(onConnection),
//                 std::move(onMessage),
//                 std::move(onClose));
//     server.start();
// }

// int main(int argc, char **argv)
// {
//     test2();
//     return 0;
// }