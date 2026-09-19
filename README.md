# LMGAMES-first
C++编码的一款以王橹杰为主角视角的一款文字修仙小游戏，目前只能在本地窗口运行，下一个项目预计使用C++加JS或py加JS的方式制作一个网页版的小游戏，或者学习一下Lua，用Lua进行开发，再接下来学习制作H5小游戏，或抖音小游戏或微信小游戏，哪个便宜做哪个，这阶段就要增加一些美术资源，还要提高一下我的画画能力好吧。再接下来采用unity制作一个功能完善的DEMO。再接下来，使用AI辅助游戏开发。 

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

这是角色初始属性和敌人属性的代码，可以在这里调整初始属性

void xiuxi() {

    jieshu();
    t+=1;
    cout<<"王橹杰感到精力充沛"<<endl;
    wanglujie.xueliang = maxxueliang;
    cout<<"王橹杰的血量回复了,当前血量:"<<wanglujie.xueliang<<"/"<<maxxueliang<<endl;
    dunwu();
}

这是休息功能的函数，可以在这里调整休息的效果，现在只有回血和顿悟效果


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

这是遭遇敌人会触发的函数，负责一来一回的战斗机制，修改部分内容可以修改战斗流程，此外，该函数里还包括战斗胜利以及战斗失败后的结算

void taopao(){}

这是遭遇敌人时会触发的函数，可以在此函数中修改逃跑的惩罚

void xiulian(){}

一个修炼的函数，可修改修炼的效果

void youli(Player mingzi){}

一个游历的函数，可以在这里调整游历时触发的事件以及相关的概率，概率采用随机数生成判断的方式

void mianban(){}

这是一个面板函数，可以在这里修改面板显示的内容以及加点的消耗

void shengji(){}

一个升级函数，可以在这里修改升级所需要的经验以及相关的文案

void dunwu(){}

触发这个函数的条件时会结束游戏

void laji(){}
void laji2(){}
void zantingchuangkou(){}

这三个函数是为了避免冗杂内容写的，方便缩减代码行数

void direnshuxing (Player diren){

}
这个函数可以使敌人的属性随天数而发生相对应的变化
可以在此基础上进行进一步的完善，接下来我打算将这个五脏俱全的小游戏做成网页版的
