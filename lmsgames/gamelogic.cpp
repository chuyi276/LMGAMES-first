// ============================================================================
// 橹穆修仙游戏 —— C++ 游戏逻辑层（pybind11 绑定）
// 原文件 lmganmes.cpp 重构：
//   1. 全部全局变量封装为 GameSession 类成员
//   2. 移除所有 cin / cout / system / Windows API
//   3. 所有文本输出改为返回 std::vector<std::string>
//   4. 战斗 / 游历中的阻塞循环拆分为非阻塞状态机
//   5. 修复原代码 direnshuxing 传值导致敌人属性不生效的 bug
//   6. 修复原代码随机范围导致部分分支永远无法触发的 bug
// ============================================================================

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cmath>

namespace py = pybind11;

// ---------------------------------------------------------------------------
// 辅助：任意数字转字符串（保留浮点数原样，不用 to_string 的固定精度）
// ---------------------------------------------------------------------------
template <typename T>
static std::string to_str(T v) {
    std::ostringstream oss;
    oss << v;
    return oss.str();
}

// ---------------------------------------------------------------------------
// 玩家 / 敌人数值结构体
// ---------------------------------------------------------------------------
struct Player {
    int dengji;          // 等级
    int chushidengji;
    double jingyan;      // 经验
    double xueliang;     // 血量
    double chushixueliang;
    int gongji;          // 攻击力
    int chushigongji;
    int tizhi;           // 体质
    int mingjie;         // 敏捷
    int wuxing;          // 悟性
    double jingshen;     // 精神
    int shuxingdian;     // 自由属性点
    std::string name;    // 名字
    double maxxl;        // 最大血量（敌人用）

    // 默认构造：主角王橹杰
    Player()
        : dengji(1), jingyan(0), xueliang(110), gongji(1),
          tizhi(1), mingjie(5), wuxing(4), jingshen(10),
          shuxingdian(0), name("王橹杰"), maxxl(110) {}

    // 按类型构造敌人：1=力  2=血  3=敏
    Player(int type) {
        dengji = 1; jingyan = 0; tizhi = 0; wuxing = 0;chushidengji = 1;
        jingshen = 0; shuxingdian = 0;
        if (type == 1) {
            xueliang = 10; gongji = 3; mingjie = 0; name = "力"; maxxl = 10;chushigongji = 3;chushixueliang = 10;
        } else if (type == 2) {
            xueliang = 20; gongji = 1; mingjie = 2; name = "血"; maxxl = 20;chushigongji  = 1;chushixueliang = 20;
        } else if (type == 3) {
            xueliang = 5; gongji = 5; mingjie = 50; name = "敏"; maxxl = 5;chushixueliang = 5;chushigongji = 5;
        } 
        // else {
        //     xueliang = 10; gongji = 1; mingjie = 0; name = "未知"; maxxl = 10;chushixueliang = 10;chushigongji = 1;
        // }
    }
};

// ---------------------------------------------------------------------------
// 一局游戏的完整会话（替代原全部全局变量 + 全局函数）
// ---------------------------------------------------------------------------
class GameSession {
public:
    // ---- 游戏全局数值 ----
    int t = 1;                  // 天数
    const int maxdengji = 27;   // 最大等级
    double maxjingyan = 100;    // 升级所需经验
    double shangbi = 10;        // 闪避相关（原代码保留）
    int dunwu1 = 10;            // 顿悟阈值
    float jingyanbeilv = 1.0f;  // 经验倍率
    double maxxueliang = 110;   // 主角最大血量
    double zongjingyan = 0;     // 累计总经验
    int kill = 0;               // 总击杀数
    int tianshu = 0; 
    // int chushi;           // 敌人初始属性

    // ---- 剧情进度 lm1~lm4 ----
    std::string lm1 = "【未解锁】", lm2 = "【未解锁】",lm3 = "【未解锁】",lm4 = "【未解锁】";

    // ---- 角色 ----
    Player wanglujie;     // 主角
    Player enemy_li{1};   // 敌人模板：力
    Player enemy_xue{2};  // 敌人模板：血
    Player enemy_min{3};  // 敌人模板：敏

    // ---- 战斗状态机 ----
    bool in_battle = false;            // 是否正在战斗中
    bool battle_pending_choice = false; // 游历遇到敌人后等待 a/s 选择
    Player current_enemy;              // 当前战斗的敌人（副本）

    // ---- 游戏结束 ----
    bool game_over = false;
    std::string game_over_msg;

    GameSession() {
        srand((unsigned int)time(NULL));
    }

    // =======================================================================
    // 私有内部方法（不导出给 Python）
    // =======================================================================
private:
    // 敌人属性随天数成长（修复：传引用而非传值）
    void direnshuxing(Player& diren) {
        diren.dengji = diren.chushidengji *(rand() % (t / 3 + 1) + rand() % 10) ;
        if (diren.dengji <= 0) diren.dengji = 1;
        diren.gongji = diren.chushidengji * diren.dengji;
        diren.xueliang = diren.chushixueliang * diren.dengji * 1.1;
        diren.maxxl = diren.xueliang;
    }

    // 升级判定
    void shengji(std::vector<std::string>& msgs) {
        while (wanglujie.jingyan >= maxjingyan && wanglujie.dengji < maxdengji) {
            wanglujie.dengji += 1;
            wanglujie.shuxingdian += 3;
            maxxueliang += 10;
            wanglujie.gongji += 1;
            wanglujie.xueliang = maxxueliang;
            msgs.push_back("王橹杰升级了！当前等级" + to_str(wanglujie.dengji)
                           + "级  属性点： " + to_str(wanglujie.shuxingdian));
            if (wanglujie.dengji <= 11) {
                wanglujie.jingyan = wanglujie.jingyan - maxjingyan;
                maxjingyan += 100;
            } else {
                wanglujie.jingyan = wanglujie.jingyan - maxjingyan;
                maxjingyan += 1000;
            }
        }
    }

    // 顿悟
    void dunwu(std::vector<std::string>& msgs) {
        int suiji = rand() % 40;  // 修复原代码 rand()%51+50 范围50~100却判断<=10的bug
        if (suiji <= dunwu1 % 2) {
            msgs.push_back("一丝灵光从王橹杰的脑中浮现");
            msgs.push_back("王橹杰的属性点增加了！");
            wanglujie.shuxingdian += 1;
        } else if (suiji <= dunwu1) {
            msgs.push_back("王橹杰好像看到了他,他摸了摸王橹杰的头，王橹杰感到异常轻松");
            msgs.push_back("王橹杰的经验增加了");
            if (wanglujie.jingyan == 0) {
                double add = 10 * wanglujie.jingshen / 10.0;
                wanglujie.jingyan += add;
                zongjingyan += add;
            } else {
                int q = (int)wanglujie.jingyan;
                wanglujie.jingyan *= 1.1;
                zongjingyan += q * 0.1;
            }
            shengji(msgs);
        }
        msgs.push_back("第" + to_str(t) + "天了");
    }

    // 结束判定
    void check_game_end(std::vector<std::string>& msgs) {
        if (t > 2711) {
            game_over = true;
            game_over_msg = "穆祉丞，你哪里去了，我怎么找不到你了.............\n"
                            "===========================游戏结束===========================";
            msgs.push_back(game_over_msg);
        }
        if (lm4 == " 【已解锁】 ") {
            game_over = true;
            game_over_msg = "王橹杰终于和穆祉丞过上幸福美满的夫夫生活了！再也没有人质疑他们之间的爱情了，"
                            "王橹杰每天看着他的全世界在草坪上奔跑，开心极了，让他慢一点，注意一下.\n"
                            "================================游戏结束================================";
            msgs.push_back(game_over_msg);
        }
    }

    // 逃跑逻辑
    void taopao(std::vector<std::string>& msgs) {
        msgs.push_back("王橹杰损失了部分经验");
        wanglujie.jingyan *= 0.8;
        int suiji = rand() % 101;  // 修复原代码 rand()%51+50 永远>=50 的bug
        if (suiji >= 50) {
            int q = rand() % 4;  // 修复原代码 rand()%2+2 永远>=2 的 bug
            if (q == 0) {
                msgs.push_back("逃跑时受到追击，血量减少");
                if (wanglujie.xueliang > maxxueliang / 2) {
                    wanglujie.xueliang *= 0.90;
                } else {
                    wanglujie.xueliang -= 20;
                    if (wanglujie.xueliang < 1) wanglujie.xueliang = 1;
                }
            }
        }
    }

    // 从三个敌人中随机选一个，设置为 current_enemy
    void pick_random_enemy() {
        int r = rand() % 3;
        if (r == 0)      current_enemy = enemy_li;
        else if (r == 1) current_enemy = enemy_xue;
        else             current_enemy = enemy_min;
    }

    // =======================================================================
    // 公开方法（导出给 Python 调用）
    // =======================================================================
public:
    // ---- 开场文本 ----
    std::vector<std::string> get_intro() {
        std::vector<std::string> msgs;
        msgs.push_back(
            "这天晚上，你正在床上刷着“天魔”“内娱唯一真情侣”“暗恋真情侣”“国一美帝”等相关的橹穆视频，"
            "时间飞速流转，光重新回到了地面,而你却感觉不到一丝疲惫,只因为你找到一款由Feris Mu老师制作的"
            "一款橹穆修仙小游戏,一直玩到实在受不了了,再次睁眼时,你发现王橹杰居然站在你的身前,他似乎看不见你, "
            "他的眼中透露着一丝惊奇,他的手指头在你的面前点来点去,你感到十分疑惑,感觉他好像有点像游戏里的人物一样, "
            "一个大胆的想法出现在你的脑海中,你学着游戏里的样子,心中默念“面板”(按1打开面板)"
            "(一个蓝粉色面板出现在你眼前)你发现自己可以给王橹杰发布任务,你暗下决心,你一定要让橹穆成为内娱的王。");
        msgs.push_back("======================游戏开始=====================");
        msgs.push_back("注意：游戏通过或者失败时会在次日进行结算");
        msgs.push_back("1 面板  2 休息  3 修炼  4 游历  9 退出");
        // msgs.push_back("【面板加点】a 体质  s 敏捷  d 悟性(耗3点)  f 精神  z 返回");
        // msgs.push_back("【战斗中】a=攻击  s=逃跑");
        msgs.push_back("祝您游戏愉快");
        msgs.push_back("================================================");
        return msgs;
    }

    // ---- 查看面板 ----
    std::vector<std::string> get_panel_info() {
        std::vector<std::string> msgs;
        msgs.push_back("===== 王橹杰的属性面板 =====");
        msgs.push_back("等级: " + to_str(wanglujie.dengji) + "/" + to_str(maxdengji));
        msgs.push_back("经验: " + to_str(wanglujie.jingyan) + "/" + to_str(maxjingyan));
        msgs.push_back("血量: " + to_str(wanglujie.xueliang) + "/" + to_str(maxxueliang)
                       + "   攻击力: " + to_str(wanglujie.gongji));
        msgs.push_back("体质: " + to_str(wanglujie.tizhi) + " (与血量和攻击力有关)");
        msgs.push_back("敏捷: " + to_str(wanglujie.mingjie) + " (与闪避几率有关)");
        msgs.push_back("悟性: " + to_str(wanglujie.wuxing) + " (与顿悟概率有关)");
        msgs.push_back("精神: " + to_str(wanglujie.jingshen) + " (与经验获取倍率有关)");
        msgs.push_back("自由属性点: " + to_str(wanglujie.shuxingdian));
        msgs.push_back("总杀敌数: " + to_str(kill) + "   累计总经验: " + to_str(zongjingyan));
        msgs.push_back("当前是第 " + to_str(t) + " 天");
        msgs.push_back("成就: I DONT CARE --- " + lm1);
        msgs.push_back("落叶知我意，天公协美助 --- " + lm2);
        msgs.push_back("爱夫者风生水起 --- " + lm3);
        msgs.push_back("世纪婚礼 --- " + lm4);
        // msgs.push_back("‘0表示尚未解锁’");
        return msgs;
    }

    // ---- 休息 ----
    std::vector<std::string> action_xiuxi() {
        std::vector<std::string> msgs;
        if (game_over)            { msgs.push_back("游戏已结束"); return msgs; }
        if (in_battle)            { msgs.push_back("战斗中无法休息"); return msgs; }
        if (battle_pending_choice){ msgs.push_back("请先选择战斗或逃跑"); return msgs; }

        if (t / 6 == 0) {
            tianshu += 1;
            msgs.push_back("一个神秘人在王橹杰睡觉时在床边看着王橹杰，王橹杰似乎感觉到了，"
                           "睁眼时却什么也没看到，王橹杰有所思考");
            msgs.push_back("王橹杰的体质增加了！！！");
            wanglujie.tizhi += tianshu;
        }
        check_game_end(msgs);
        if (game_over) return msgs;

        t += 1;
        msgs.push_back("王橹杰感到精力充沛");
        wanglujie.xueliang = maxxueliang;
        msgs.push_back("王橹杰的血量回复了,当前血量:" + to_str(wanglujie.xueliang)
                       + "/" + to_str(maxxueliang));
        dunwu(msgs);
        check_game_end(msgs);
        return msgs;
    }

    // ---- 修炼 ----
    std::vector<std::string> action_xiulian() {
        std::vector<std::string> msgs;
        if (game_over)            { msgs.push_back("游戏已结束"); return msgs; }
        if (in_battle)            { msgs.push_back("战斗中无法修炼"); return msgs; }
        if (battle_pending_choice){ msgs.push_back("请先选择战斗或逃跑"); return msgs; }

        if (t / 11 == 0) {
            tianshu += 1;
            msgs.push_back("一个神秘人在王橹杰修炼时在不远处看着王橹杰，王橹杰似乎感觉到了，"
                           "睁眼时却什么也没看到，王橹杰有所思考");
            msgs.push_back("王橹杰的悟性增加了！！！");
            wanglujie.wuxing += tianshu;
        }
        check_game_end(msgs);
        if (game_over) return msgs;

        t += 1;
        msgs.push_back("王橹杰的经验增加了");
        double add = 100 * wanglujie.jingshen / 10.0;
        zongjingyan += add;
        wanglujie.jingyan += add;
        shengji(msgs);
        dunwu(msgs);
        check_game_end(msgs);
        return msgs;
    }

    // ---- 游历（可能遇到敌人，进入待选择状态） ----
    std::vector<std::string> action_youli() {
        std::vector<std::string> msgs;
        if (game_over)            { msgs.push_back("游戏已结束"); return msgs; }
        if (in_battle)            { msgs.push_back("已在战斗中"); return msgs; }
        if (battle_pending_choice){ msgs.push_back("请先选择战斗或逃跑"); return msgs; }

        // 先刷新三个敌人的属性（随天数成长）
        direnshuxing(enemy_li);
        direnshuxing(enemy_xue);
        direnshuxing(enemy_min);

        // ---- 剧情 lm1 → lm2 → lm3 → lm4 逐步触发 ----
        // 修复原代码 rand()%51+50 范围50~100却判断==10的bug，改为0~50
        int dddd = rand() % 51;
        if (dddd == 10&&wanglujie.dengji>4&&wanglujie.wuxing>=5) {
            lm1 = " 【已解锁】 ";
            msgs.push_back("落叶知秋，我以为你不说声再见就走了");
            msgs.push_back("=================这下再也走不掉了好像==================");
        }
        if (lm1 == " 【已解锁】 ") {
            int d1 = rand() % 51;
            if (d1 == 10&&wanglujie.dengji>7&&wanglujie.tizhi>=15) {
                lm2 = " 【已解锁】 ";
                msgs.push_back("祝你星途璀璨");
                msgs.push_back("==================这下祝福真的成真了=================");
            }
        }
        if (lm2 == " 【已解锁】 ") {
            int d1 = rand() % 51;
            if (d1 == 10&&wanglujie.dengji>10&&wanglujie.mingjie>15) {
                lm3 = " 【已解锁】 ";
                msgs.push_back("紧张吗？");
                msgs.push_back("有点");
                msgs.push_back("=================一段对话浮现在你脑海中==================");
            }
        }
        if (lm3 == " 【已解锁】 ") {
            int d1 = rand() % 51;
            if (d1 == 10&&wanglujie.dengji>20&&wanglujie.jingshen>=30) {
                lm4 = " 【已解锁】 ";
                msgs.push_back("游戏的故事结束了，但现实仍然在滚滚前行");
                msgs.push_back("===============你的脑海中忽然蹦出这样一句话===============");
            }
        }
        check_game_end(msgs);
        if (game_over) return msgs;

        // ---- 游历结果（修复原代码 rand()%2+2 只能取2/3 的 bug，改为 0~3） ----
        int q = rand() % 4;
        if (q == 0) {
            // 满怀期待出发
            msgs.push_back("王橹杰满怀期待的出发了");
            pick_random_enemy();
            msgs.push_back("王橹杰遇到了" + current_enemy.name);
            if (wanglujie.gongji < 100) wanglujie.gongji += 4;
            else wanglujie.gongji = (int)(wanglujie.gongji * 1.05);
            msgs.push_back("按 a 开始战斗，按 s 逃跑");
            battle_pending_choice = true;
        } else if (q == 1) {
            msgs.push_back("王橹杰失落的出发了");
            pick_random_enemy();
            msgs.push_back("王橹杰遇到了" + current_enemy.name);
            if (wanglujie.mingjie >= 10) {
                wanglujie.mingjie -= 1;
                msgs.push_back("按 a 开始战斗，按 s 逃跑");
                battle_pending_choice = true;
                wanglujie.mingjie+=1;
            } else {
                msgs.push_back("按 a 开始战斗，按 s 逃跑");
                battle_pending_choice = true;
            }
            
        } else if (q == 2) {
            pick_random_enemy();
            msgs.push_back("王橹杰在路上遇到了" + current_enemy.name);
            msgs.push_back("按 a 开始战斗，按 s 逃跑");
            battle_pending_choice = true;
        } else {
            msgs.push_back("无事发生");
            msgs.push_back("王橹杰的经验增加了！");
            int q2 = (int)wanglujie.jingyan;
            wanglujie.jingyan *= (1.05 * wanglujie.jingshen / 10.0);
            zongjingyan += (q2 * 0.05 * wanglujie.jingshen / 10.0);
            shengji(msgs);
        }
        return msgs;
    }

    // ---- 加点（a=体质 s=敏捷 d=悟性 f=精神） ----
    std::vector<std::string> add_point(char op) {
        std::vector<std::string> msgs;
        if (game_over)            { msgs.push_back("游戏已结束"); return msgs; }
        if (in_battle)            { msgs.push_back("战斗中无法加点"); return msgs; }
        if (battle_pending_choice){ msgs.push_back("请先选择战斗或逃跑"); return msgs; }

        if (op == 'a') {
            if (wanglujie.shuxingdian > 0) {
                msgs.push_back("体质+1");
                wanglujie.tizhi += 1;
                maxxueliang += 10;
                wanglujie.gongji += 1;
                wanglujie.shuxingdian -= 1;
            } else msgs.push_back("属性点不足");
        } else if (op == 's') {
            if (wanglujie.shuxingdian > 0 && wanglujie.mingjie < 100) {
                msgs.push_back("敏捷+1");
                wanglujie.mingjie += 1;
                shangbi += 1;
                wanglujie.shuxingdian -= 1;
            } else if (wanglujie.mingjie >= 100) msgs.push_back("敏捷已达到上限，无法提高");
            else msgs.push_back("属性点不足");
        } else if (op == 'd') {
            if (wanglujie.shuxingdian > 2 && wanglujie.wuxing < 100) {
                msgs.push_back("悟性+1");
                wanglujie.wuxing += 1;
                dunwu1 += 10;
                wanglujie.shuxingdian -= 3;
            } else if (wanglujie.wuxing >= 100) msgs.push_back("悟性已达到上限，无法提高");
            else msgs.push_back("属性点不足或者点数不够(悟性需要3点)");
        } else if (op == 'f') {
            if (wanglujie.shuxingdian > 0) {
                msgs.push_back("精神+1");
                wanglujie.jingshen += 1;
                jingyanbeilv += 0.1f;
                wanglujie.shuxingdian -= 1;
            } else msgs.push_back("属性点不足");
        } else {
            msgs.push_back("输入无效！请输入 a体质 s敏捷 d悟性 f精神");
        }
        msgs.push_back("当前属性：攻" + to_str(wanglujie.gongji)
                       + " 体" + to_str(wanglujie.tizhi)
                       + " 敏" + to_str(wanglujie.mingjie)
                       + " 悟" + to_str(wanglujie.wuxing)
                       + " 精" + to_str(wanglujie.jingshen)
                       + " | 剩余点:" + to_str(wanglujie.shuxingdian));
        return msgs;
    }

    // ---- 确认开始战斗（从待选择状态进入战斗） ----
    std::vector<std::string> battle_start() {
        std::vector<std::string> msgs;
        if (!battle_pending_choice) {
            msgs.push_back("当前没有可战斗的敌人");
            return msgs;
        }
        in_battle = true;
        battle_pending_choice = false;
        msgs.push_back("===== 战斗开始！ =====");
        msgs.push_back(current_enemy.name + "  等级" + to_str(current_enemy.dengji)
                       + "  血量:" + to_str(current_enemy.xueliang) + "/" + to_str(current_enemy.maxxl)
                       + "  攻击力:" + to_str(current_enemy.gongji));
        msgs.push_back("按 a 攻击，按 s 逃跑");
        return msgs;
    }

    // ---- 战斗中攻击一次（非阻塞，执行一回合） ----
    std::vector<std::string> battle_attack() {
        std::vector<std::string> msgs;
        if (!in_battle) { msgs.push_back("当前不在战斗中"); return msgs; }

        // 互相扣血
        wanglujie.xueliang -= current_enemy.gongji;
        current_enemy.xueliang -= wanglujie.gongji;
        if(rand() % 100<=current_enemy.mingjie){
            current_enemy.xueliang+=wanglujie.gongji;
            msgs.push_back("王橹杰的攻击被躲开了"); 
        }
        if(rand() % 100<=wanglujie.mingjie){
            wanglujie.xueliang+=current_enemy.gongji;
            msgs.push_back("王橹杰进行了一次完美扭腰，躲过了攻击"); 
        }
        if(current_enemy.xueliang<=0){current_enemy.xueliang = 0;}
        msgs.push_back(current_enemy.name + "  血量:" + to_str(current_enemy.xueliang)
                       + "/" + to_str(current_enemy.maxxl) + "  攻击力:" + to_str(current_enemy.gongji));
        msgs.push_back(wanglujie.name + "  血量:" + to_str(wanglujie.xueliang)
                       + "/" + to_str(maxxueliang) + "  攻击力:" + to_str(wanglujie.gongji));

        // 敌人死 & 玩家活 → 胜利
        if (current_enemy.xueliang <= 0 && wanglujie.xueliang > 0) {
            msgs.push_back("===== 战斗胜利！ =====");
            msgs.push_back("经验增加了");
            kill += 1;
            int q = (int)wanglujie.jingyan;
            wanglujie.jingyan += (maxjingyan * 0.1 * wanglujie.jingshen / 10);
            maxjingyan += (wanglujie.jingyan - q);
            shengji(msgs);
            // 胜利后随机奖励属性（修复原代码 rand()%51+50 却判断<=10 的bug）
            int suiji = rand() % 30;
            if (suiji <= 10) {
                if (suiji < 3) {
                    msgs.push_back("王橹杰的体质提高了！");
                    wanglujie.tizhi += 1;
                } else if (suiji > 2 && suiji < 5) {
                    msgs.push_back("王橹杰的敏捷提高了！");
                    wanglujie.mingjie += 1;
                } else if (suiji > 5 && suiji < 10) {
                    msgs.push_back("王橹杰的精神提高了！");
                    wanglujie.jingshen += 1;
                }
            }
            in_battle = false;
            check_game_end(msgs);
        }
        // 玩家死 → 失败
        else if (wanglujie.xueliang <= 0) {
            t += 1;
            msgs.push_back("===== 战斗失败！王橹杰被一个路过的男人带回去治疗了 =====");
            wanglujie.jingyan *= 0.8;
            shengji(msgs);
            msgs.push_back("王橹杰的经验减少了");
            msgs.push_back("第" + to_str(t) + "天了");
            wanglujie.xueliang = maxxueliang;  // 治疗后回满
            in_battle = false;
            check_game_end(msgs);
        }
        // 双方都活着 → 继续
        else {
            msgs.push_back("按 a 继续攻击，按 s 逃跑");
        }
        return msgs;
    }

    // ---- 战斗中逃跑（或待选择时逃跑） ----
    std::vector<std::string> battle_flee() {
        std::vector<std::string> msgs;
        if (!in_battle && !battle_pending_choice) {
            msgs.push_back("当前不在战斗中");
            return msgs;
        }
        battle_pending_choice = false;
        taopao(msgs);
        in_battle = false;
        return msgs;
    }
};

// ===========================================================================
// pybind11 模块绑定
// ===========================================================================
PYBIND11_MODULE(gamelogic, m) {
    m.doc() = "橹穆修仙小游戏";

    py::class_<Player>(m, "Player")
        .def(py::init<>())
        .def(py::init<int>())
        .def_readwrite("dengji",      &Player::dengji)
        .def_readwrite("jingyan",     &Player::jingyan)
        .def_readwrite("xueliang",    &Player::xueliang)
        .def_readwrite("gongji",      &Player::gongji)
        .def_readwrite("tizhi",       &Player::tizhi)
        .def_readwrite("mingjie",     &Player::mingjie)
        .def_readwrite("wuxing",      &Player::wuxing)
        .def_readwrite("jingshen",    &Player::jingshen)
        .def_readwrite("shuxingdian", &Player::shuxingdian)
        .def_readwrite("name",        &Player::name)
        .def_readwrite("maxxl",       &Player::maxxl);

    py::class_<GameSession>(m, "GameSession")
        .def(py::init<>())
        // 只读数值
        .def_readonly("t",                    &GameSession::t)
        .def_readonly("maxdengji",            &GameSession::maxdengji)
        .def_readonly("maxjingyan",           &GameSession::maxjingyan)
        .def_readonly("maxxueliang",          &GameSession::maxxueliang)
        .def_readonly("zongjingyan",          &GameSession::zongjingyan)
        .def_readonly("kill",                 &GameSession::kill)
        .def_readwrite("lm1",                  &GameSession::lm1)
        .def_readwrite("lm2",                  &GameSession::lm2)
        .def_readwrite("lm3",                  &GameSession::lm3)
        .def_readwrite("lm4",                  &GameSession::lm4)
        .def_readonly("in_battle",            &GameSession::in_battle)
        .def_readonly("battle_pending_choice",&GameSession::battle_pending_choice)
        .def_readonly("game_over",            &GameSession::game_over)
        .def_readonly("game_over_msg",        &GameSession::game_over_msg)
        .def_readonly("wanglujie",            &GameSession::wanglujie)
        .def_readonly("current_enemy",        &GameSession::current_enemy)
        // 方法
        .def("get_intro",      &GameSession::get_intro)
        .def("get_panel_info", &GameSession::get_panel_info)
        .def("action_xiuxi",   &GameSession::action_xiuxi)
        .def("action_xiulian", &GameSession::action_xiulian)
        .def("action_youli",   &GameSession::action_youli)
        .def("add_point",      &GameSession::add_point)
        .def("battle_start",   &GameSession::battle_start)
        .def("battle_attack",  &GameSession::battle_attack)
        .def("battle_flee",    &GameSession::battle_flee);
}
//（注：内容由AI生成加上主播的后续修改好吧，豆包审美太垃圾了，以后换个别的nb的ai，不过其他的都要翻墙好像，怕中病毒，先用豆包，kimi，够我用了,后面找时间优化一下美术，搞成蓝粉色的好吧嘿嘿）
