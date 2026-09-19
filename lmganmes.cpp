#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

int t = 1;
int maxdengji = 27;
double maxjingyan = 100;
double shangbi = 10;
int dunwu1 = 10;
float jingyanbeilv = 1.0;
double maxxueliang = 110;
int suiji1 = 0;
double zongjingyan = 0;
int kill = 0;

int lm1 = 0,lm2 = 0,lm3 = 0,lm4 = 0;;

struct Player {
    int dengji;
    double jingyan;
    double xueliang;
    int gongji;
    int tizhi;
    int mingjie;
    int wuxing;
    double jingshen;
    int shuxingdian;
    string name;
    double maxxl;

    Player() : name("王橹杰"),dengji(1), jingyan(0), xueliang(110), gongji(1), tizhi(1), mingjie(5), wuxing(4), jingshen(10), shuxingdian(0) {}

    // 构造函数2：a1模板
    Player(int type) {
        if(type == 1) {
            dengji = 1;
            jingyan = 0;
            xueliang = 10;
            gongji = 3;
            tizhi = 0;
            mingjie = 0;
            wuxing = 0;
            jingshen = 0;
            shuxingdian = 0;
            name = "a1";
            maxxl = 10;
        }
        else if(type == 2) {
            dengji = 1;
            jingyan = 0;
            xueliang = 20;
            gongji = 1;
            tizhi = 0;
            mingjie = 2;
            wuxing = 0;
            jingshen = 0;
            shuxingdian = 0;
            name = "a2";
            maxxl = 20;
        }
        else if(type ==3) {
            dengji = 1;
            jingyan = 0;
            xueliang = 5;
            gongji = 5;
            tizhi = 0;
            mingjie = 50;
            wuxing = 0;
            jingshen = 0;
            shuxingdian = 0;
            name = "a3";
            maxxl = 5;
        }
    }
};

// 创建4个角色实例
Player wanglujie;   // 默认构造
Player a1(1);
Player a2(2);
Player a3(3);

void xiuxi();
void xiulian();
void youli(Player mingzi);
void mianban();
void shengji();
void dunwu();
void zhandou(int d,Player mingzi);
void taopao();
void jieshu();
void laji();
void laji2();
void direnshuxing(Player diren);
void zantingchuangkou();

int main(){
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    srand((unsigned int)time(NULL));
    cout<<"这天晚上，你正在床上刷着“天魔”“内娱唯一真情侣”“暗恋真情侣”“国一美帝”等相关的橹穆视频，时间飞速流转，光重新回到了地面,而你却感觉不到一丝疲惫,只因为你找到一款由Feris Mu老师制作的一款橹穆修仙小游戏,一直玩到实在受不了了,再次睁眼时,你发现王橹杰居然站在你的身前,他似乎看不见你,他的眼中透露着一丝惊奇,他的手指头在你的面前点来点去,你感到十分疑惑,感觉他好像有点像游戏里的人物一样,一个大胆的想法出现在你的脑海中,你学着游戏里的样子,心中默念“面板”(按1打开面板)(一个蓝粉色面板出现在你眼前)你发现自己可以给王橹杰发布任务,你暗下决心,你一定要让橹穆成为内娱的王。"<<endl;
    cout<<"==========================游戏开始=========================="<<endl;
    cout<<"注意：游戏通过或者失败时会在次日进行结算"<<endl;
    cout<<"1 查看面板"<<endl;
    cout<<"2 休息"<<endl;
    cout<<"3 修炼（概率增加属性点）"<<endl;
    cout<<"4 游历（概率触发特殊事件）"<<endl;
    cout<<"9 退出游戏"<<endl;
    cout<<"祝您游戏愉快"<<endl;
    cout<<"============================================================"<<endl;

    char op;
    while(true){
        jieshu();
        cin>>op;
        switch(op){
            case '9':
                cout<<"哥哥你哪里去了，我找不到你了哥哥，我好想你"<<endl;
                cout<<"游戏结束"<<endl;
                zantingchuangkou();
                return 0;
            case '1':
                mianban();
                break;
            case '2':
                xiuxi();
                cout<<"王橹杰接下来要做什么："<<endl;
                cout<<"1 查看面板 ";
                cout<<"2 休息 ";
                cout<<"3 修炼（概率增加属性点）";
                cout<<"4 游历（概率触发特殊事件）";
                cout<<"9 退出游戏";
                break;
            case '3':
                xiulian();
                cout<<"王橹杰接下来要做什么："<<endl;
                cout<<"1 查看面板 ";
                cout<<"2 休息 ";
                cout<<"3 修炼（概率增加属性点）";
                cout<<"4 游历（概率触发特殊事件）";
                cout<<"9 退出游戏";
                break;
            case '4':
                suiji1 = rand()% 2 + 1;
                if(suiji1==0){
                    youli(a1);
                    break;
                }
                if(suiji1==1){
                    youli(a2);
                    break;
                }
                if(suiji1==2){
                    youli(a3);
                    break;
                }
            default:
                cout<<"输入无效！请输入1查看面板，2休息，3修炼，4游历，9退出游戏"<<endl;
                break;
        }
    }
    zantingchuangkou();
    return 0;
}

//休息
void xiuxi() {

    jieshu();
    t+=1;
    cout<<"王橹杰感到精力充沛"<<endl;
    wanglujie.xueliang = maxxueliang;
    cout<<"王橹杰的血量回复了,当前血量:"<<wanglujie.xueliang<<"/"<<maxxueliang<<endl;
    dunwu();
}

//战斗
void zhandou(Player mingzi){
    while(true){
        cout<<"王橹杰的属性面板"<<"  ";
        cout<<"等级: "<<wanglujie.dengji<<"/"<<maxdengji<<"  ";
        cout<<"经验: "<<wanglujie.jingyan<<"/"<<maxjingyan<<"  ";
        cout<<"血量："<<wanglujie.xueliang<<"/"<<maxxueliang<<"  ";
        cout<<"攻击力: "<<wanglujie.gongji<<"  ";
        cout<<"体质: "<<wanglujie.tizhi<<"  ";
        cout<<"敏捷: "<<wanglujie.mingjie<<"  ";
        cout<<"悟性: "<<wanglujie.wuxing<<"  ";
        cout<<"精神: "<<wanglujie.jingshen<<endl<<"  ";
        cout<<mingzi.name<<":等级"<<mingzi.dengji<<" 血量:"<<mingzi.xueliang<<'/'<<mingzi.maxxl<<" 攻击力:"<<mingzi.gongji<<endl;
        cout<<"是否对"<<mingzi.name<<"进行攻击"<<"按a确认"<<endl;
        cout<<"按s逃跑"<<endl;
        char bb;
        while(true){
            
            cin>>bb;
            cout<<"是否对"<<mingzi.name<<"进行攻击"<<"按a确认"<<endl;
            cout<<"按s逃跑"<<endl;
            switch(bb){
                case 's' :
                    taopao();
                    return;
            
                case 'a' ://相互攻击并扣血，实时显示双方血量
                    wanglujie.xueliang-=mingzi.gongji;
                    mingzi.xueliang-=wanglujie.gongji;
                    cout<<mingzi.name<<":等级"<<mingzi.dengji<<" 血量:"<<mingzi.xueliang<<"/"<<mingzi.maxxl<<" 攻击力:"<<mingzi.gongji<<endl;
                    cout<<wanglujie.name<<":等级"<<wanglujie.dengji<<" 血量:"<<wanglujie.xueliang<<"/"<<maxxueliang<<" 攻击力:"<<wanglujie.gongji<<endl;
                    if(mingzi.xueliang<=0&&wanglujie.xueliang>0){
                        cout<<"战斗胜利！"<<endl<<"经验增加了"<<endl;
                        kill+=1;
                        if(wanglujie.jingyan>=0){
                            int q = wanglujie.jingyan;
                            wanglujie.jingyan+=(maxjingyan*0.1*wanglujie.jingshen/10);
                            maxjingyan+=(wanglujie.jingyan-q);
                        }
                        shengji();
                        int suiji = rand() % 51 + 50;
                        if(suiji<=10){
                            if(suiji<3){
                                cout<<"王橹杰的体质提高了！"<<endl;
                                wanglujie.tizhi+=1;
                            }
                            else if(suiji>2&&suiji<5){
                                cout<<"王橹杰的敏捷提高了！"<<endl;
                                wanglujie.mingjie+=1;
                            }
                            else if(suiji>5&&suiji<10){
                                cout<<"王橹杰的精神提高了！"<<endl;
                                wanglujie.jingshen+=1;
                            }
                            return;
                        }
                    }
                    if(wanglujie.xueliang<=0){
                        t+=1;
                        cout<<"战斗失败！王橹杰被一个路过的男人带回去治疗了"<<endl;
                        wanglujie.jingyan*=0.8;
                        shengji();
                        cout<<"王橹杰的经验减少了"<<endl;
                        cout<<"第"<<t<<"天了";
                        cout<<"王橹杰接下来要做什么："<<endl;
                        cout<<"1 查看面板 ";
                        cout<<"2 休息 ";
                        cout<<"3 修炼（概率增加属性点）";
                        cout<<"4 游历（概率触发特殊事件）";
                        cout<<"9 退出游戏";
                        return;
                    }
                default:
                    cout<<"输入无效！请输入a攻击，s逃跑"<<endl;
                    break;    
            }
        }
    }
}

//逃跑
void taopao(){
    cout<<"王橹杰损失了部分经验"<<endl;
    wanglujie.jingyan*=0.8;
    cout<<"王橹杰接下来要做什么："<<endl;
    cout<<"1 查看面板 ";
    cout<<"2 休息 ";
    cout<<"3 修炼（概率增加属性点）";
    cout<<"4 游历（概率触发特殊事件）";
    cout<<"9 退出游戏";
    int suiji = rand() % 51 + 50;
    if(suiji>=50){
        int q = rand() % 2 + 2;
        if(q==0){
            cout<<"血量减少"<<endl;
            if(wanglujie.xueliang>maxxueliang/2){
                wanglujie.xueliang *= 0.90;                
            }
            else {
                wanglujie.xueliang-=20;
            }
            return;
        }
        else return;
    }
    return;
}

//修炼
void xiulian(){
    jieshu();
    t+=1;
    cout<<"王橹杰的经验增加了"<<endl;
    zongjingyan+=(100*wanglujie.jingshen/10.0);
    wanglujie.jingyan+=(100*wanglujie.jingshen/10.0);
    shengji();
    dunwu();
    return;
}

//游历
void youli(Player mingzi){
    direnshuxing(a1);
    direnshuxing(a2);
    direnshuxing(a3);
    char op1;
    int dddd = rand() % 51 + 50;
    if(dddd==10){
        lm1=1;
        cout<<"落叶知秋，我以为你不说声再见就走了"<<endl;
        cout<<"====================这下再也走不掉了好像===================="<<endl;
    }
    if(lm1==1){
        int dddd1 = rand() % 51 + 50;
        if(dddd1==10){
            lm2=1;
            cout<<"祝你星途璀璨"<<endl;
            cout<<"====================这下祝福真的成真了===================="<<endl;
        }
    }
    if(lm2==1){
        int dddd1 = rand() % 51 + 50;
        if(dddd1==10){
            lm3=1;
            cout<<"紧张吗？"<<endl;
            cout<<"有点"<<endl;
            cout<<"====================一段对话浮现在你脑海中===================="<<endl;
        }
    }
    if(lm3==1){
        int dddd1 = rand() % 51 + 50;
        if(dddd1==10){
            lm4=1;
            cout<<"游戏的故事结束了，但现实仍然在滚滚前行"<<endl;
            cout<<"====================你的脑海中忽然蹦出这样一句话===================="<<endl;
        }
    }
    int q = rand() % 2 + 2;
    if(q==0){
        cout<<"王橹杰满怀";
        int a = rand() % 2 + 2;
        if(a<2){
            cout<<"期待的出发了,王橹杰遇到了"<<mingzi.name;
            if(wanglujie.gongji<100){
                wanglujie.gongji+=4;
            }
            else {
                wanglujie.gongji*=1.05;
            }
            cout<<"按a开始战斗，按s逃跑";
            cin>>op1;
            switch(op1){
                case 'a' :
                    zhandou(mingzi);
                case 's' :
                    taopao();
            }
        }
        else if(a>1){
            cout<<"失落的出发了，王橹杰遇到了"<<mingzi.name<<endl;
            if(wanglujie.mingjie>=10){
                wanglujie.mingjie-=10;
                cout<<"按a开始战斗，按s逃跑";
                cin>>op1;
                switch(op1){
                    case 'a' :
                        zhandou(mingzi);
                    case 's' :
                        taopao();
                }
            }
        }
    }
    else if(q==1){
        cout<<"王橹杰";
    }
    else if(q==2){
        cout<<"王橹杰在路上遇到了"<<mingzi.name<<endl;
        cout<<"按a开始战斗，按s逃跑";
        cin>>op1;
        switch(op1){
            case 'a' :
                zhandou(mingzi);
            case 's' :
                taopao();
        }
    }
    else if(q==3){
        cout<<"无事发生"<<endl;
        cout<<"王橹杰的经验增加了！"<<endl;
        int q =wanglujie.jingyan;
        wanglujie.jingyan*=(1.05*wanglujie.jingshen/10.0);
        zongjingyan = zongjingyan + (q*0.05*wanglujie.jingshen/10.0);
        shengji();
    }
    return;
}

//面板
void mianban(){
    cout<<"王橹杰的属性面板"<<endl;
    cout<<"等级: "<<wanglujie.dengji<<"/"<<maxdengji<<endl;
    cout<<"经验: "<<wanglujie.jingyan<<"/"<<maxjingyan<<endl;
    cout<<"血量："<<wanglujie.xueliang<<"/"<<maxxueliang<<"  ";
    cout<<"攻击力: "<<wanglujie.gongji<<endl;
    cout<<"体质: "<<wanglujie.tizhi<<"(与血量和攻击力有关)"<<endl;
    cout<<"敏捷: "<<wanglujie.mingjie<<"(与闪避几率有关)"<<endl;
    cout<<"悟性: "<<wanglujie.wuxing<<"(与顿悟概率有关)"<<endl;
    cout<<"精神: "<<wanglujie.jingshen<<"(与经验获取倍率有关)"<<endl;
    cout<<"=========================================================="<<endl;
    cout<<"自由属性点: "<<wanglujie.shuxingdian<<endl;
    cout<<"总杀敌数："<<kill<<" 获取的总经验值："<<zongjingyan<<endl;
    cout<<"当前是第"<<t<<"天";
    cout<<"可添加在体质(a)，敏捷(s)，悟性(d)以及精神(f)四个属性上，其中添加悟性需要消耗3点属性点"<<endl;
    cout<<"按z退出"<<endl;
    
    char aa;
    while(true){
        
        cin>>aa;
        switch(aa){
            case 'z' :
                cout<<"王橹杰接下来要做什么：";
                cout<<"1 查看面板 ";
                cout<<"2 休息 ";
                cout<<"3 修炼（概率增加属性点）";
                cout<<"4 游历（概率触发特殊事件）";
                cout<<"9 退出游戏 ";
                return;
            case 'a' :
                if(wanglujie.shuxingdian>0){
                    cout<<"体质+1"<<endl;
                    wanglujie.tizhi+=1;
                    maxxueliang+=10;
                    wanglujie.gongji+=1;
                    wanglujie.shuxingdian-=1;
                    laji();
                    break;
                }else {
                    cout<<"属性点不足"<<endl;
                    laji2();
                    break;
                }
            case 's' :
                if(wanglujie.shuxingdian>0&&wanglujie.mingjie<100){
                    cout<<"敏捷+1"<<endl;
                    wanglujie.mingjie+=1;
                    shangbi+=1;
                    wanglujie.shuxingdian-=1;
                    laji();
                    break;
                }else if(wanglujie.mingjie>=100){
                    cout<<"敏捷已达到上限，无法提高";
                    break;
                }
                else{
                    cout<<"属性点不足"<<endl;
                    laji2();
                    break;
                }
            case 'd' :
                if(wanglujie.shuxingdian>2&&wanglujie.wuxing<100){
                    cout<<"悟性+1"<<endl;
                    wanglujie.wuxing+=1;
                    dunwu1+=10;
                    wanglujie.shuxingdian-=3;
                    laji();
                    break;
                }else if(wanglujie.shuxingdian>2&&wanglujie.wuxing>=100){
                    cout<<"悟性已达到上限，无法提高"<<endl;
                    break;
                }
                else{
                    cout<<"属性点不足"<<endl;
                    laji2();
                    break;
                }
            case 'f' :
                if(wanglujie.shuxingdian>0){
                    cout<<"精神+1"<<endl;
                    wanglujie.jingshen+=1;
                    jingyanbeilv+=0.1;
                    wanglujie.shuxingdian-=1;
                    laji();
                    break;
                }else {
                    cout<<"属性点不足"<<endl;
                    laji2();
                    break;
                }
            default:
                cout<<"输入无效！请输入a体质，s敏捷，d悟性，f精神，z退出"<<endl;
                break;
        }
}
    return;
}

//升级
void shengji(){
    if(wanglujie.jingyan>=maxjingyan){
        if(wanglujie.dengji<=maxdengji){
            wanglujie.dengji+=1;
            wanglujie.shuxingdian+=3;
            maxxueliang+=10;
            wanglujie.gongji+=1;
            wanglujie.xueliang = maxxueliang;
            cout<<"王橹杰升级了！当前等级"<<wanglujie.dengji<<"级  "<<"属性点： "<<wanglujie.shuxingdian<<endl;
            if(wanglujie.dengji<=11){
                wanglujie.jingyan = wanglujie.jingyan - maxjingyan;
                maxjingyan+=100;
            }
            else{
                wanglujie.jingyan = wanglujie.jingyan - maxjingyan;
                maxjingyan+=1000;
            }
        }
    }
    return;
}

//顿悟
void dunwu(){
    int suiji = rand() % 51 + 50;
    if(suiji<=dunwu1%2){
        cout<<"一丝灵光从王橹杰的脑中浮现"<<endl;
        cout<<"王橹杰的属性点增加了！"<<endl;
        wanglujie.shuxingdian+=1;
        cout<<"第"<<t<<"天了";
    }
    else if(suiji<=dunwu1){
        cout<<"王橹杰好像看到了他,他摸了摸王橹杰的头，王橹杰感到异常轻松"<<endl;
        cout<<"王橹杰的经验增加了"<<endl;
        if(wanglujie.jingyan==0){
            wanglujie.jingyan+=(10*wanglujie.jingshen/10.0);
            zongjingyan+=(10*wanglujie.jingshen/10.0);
        }
        else {
            int q = wanglujie.jingyan;
            wanglujie.jingyan*=1.1;
            zongjingyan+=q*0.1;
        }
        shengji();
        cout<<"第"<<t<<"天了";
    }
    else {
        cout<<"第"<<t<<"天了";
    }
    return;
}

//结束
void jieshu(){
    if(t>2711){
        cout<<"穆祉丞，你哪里去了，我怎么找不到你了............."<<endl<<"===========================游戏结束==========================="<<endl;
        zantingchuangkou();
        return;
    }
    if(lm4 == 1){
        cout<<"王橹杰终于和穆祉丞过上幸福美满的夫夫生活了！再也没有人质疑他们之间的爱情了，王橹杰每天看着他的全世界在草坪上奔跑，开心极了，让他慢一点，注意一下."<<endl;
        cout<<"================================游戏结束================================"<<endl;
        zantingchuangkou();
        return;
    }
}

//存点循环使用到的，节省代码空间
void laji(){
    cout<<"当前属性：";
    cout<<" 攻击力: "<<wanglujie.gongji;
    cout<<" 体质: "<<wanglujie.tizhi;
    cout<<" 敏捷: "<<wanglujie.mingjie;
    cout<<" 悟性: "<<wanglujie.wuxing;
    cout<<" 精神: "<<wanglujie.jingshen<<endl;
    cout<<"继续加点--体质(a)，敏捷(s)，悟性(d)以及精神(f)    z退出面板"<<endl;
}
void laji2(){
    cout<<"当前属性：";
    cout<<" 攻击力: "<<wanglujie.gongji;
    cout<<" 体质: "<<wanglujie.tizhi;
    cout<<" 敏捷: "<<wanglujie.mingjie;
    cout<<" 悟性: "<<wanglujie.wuxing;
    cout<<" 精神: "<<wanglujie.jingshen<<endl;
    cout<<"按z退出"<<endl;
}

//敌人属性变更，随天数增加而增加
void direnshuxing (Player diren){
    diren.dengji = rand() % t/3 + 1;
    if(diren.dengji<=0){
        diren.dengji=1;
    }
    diren.maxxl = diren.xueliang;
    diren.gongji*=diren.dengji;
    diren.xueliang = diren.xueliang*diren.dengji*1.1;
    
}

//暂停窗口
void zantingchuangkou(){
    cout << "\n按回车键关闭窗口..." << endl;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //清空缓冲区
    cin.get(); 
}

//不设置结局了，留白兄弟，写个开放结局，lm还是进行时。好吧