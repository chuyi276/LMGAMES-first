# -*- coding: utf-8 -*-
"""
橹穆修仙游戏 —— Python 图形界面层（pygame）
架构：C++ (gamelogic) 负责全部游戏数值/逻辑/战斗
      Python (pygame) 负责窗口渲染、输入、消息显示
"""

import sys
import os
import pygame
import gamelogic

# ===========================================================================
# 资源路径处理（兼容 PyInstaller 打包后的多种路径）
# ===========================================================================
def resource_path(relative_path):
    if hasattr(sys, '_MEIPASS'):
        p = os.path.join(sys._MEIPASS, relative_path)
        if os.path.exists(p):
            return p
    if getattr(sys, 'frozen', False):
        exe_dir = os.path.dirname(sys.executable)
        p = os.path.join(exe_dir, relative_path)
        if os.path.exists(p):
            return p
        p = os.path.join(exe_dir, '_internal', relative_path)
        if os.path.exists(p):
            return p
    p = os.path.join(os.path.dirname(os.path.abspath(__file__)), relative_path)
    if os.path.exists(p):
        return p
    return os.path.join(os.getcwd(), relative_path)


# ===========================================================================
# 常量
# ===========================================================================
WIN_W, WIN_H = 1135, 800
FPS = 60


# 135, 206, 235 天蓝色
# 255, 192, 203 浅粉色
BG_COLOR        = (22, 20, 35)
PANEL_BG        = (38, 34, 58)
PANEL_BORDER    = (120, 100, 180)
LOG_BG          = (28, 26, 42)
LOG_BORDER      = (80, 70, 120)
BTN_NORMAL      = (70, 60, 110)
BTN_HOVER       = (110, 90, 170)
BTN_PRESS       = (140, 110, 200)
BTN_TEXT        = (240, 235, 255)
BTN_DISABLED    = (50, 48, 60)
BTN_DIS_TEXT    = (120, 115, 130)
TITLE_COLOR     = (255, 210, 120) #左上角王橹杰的颜色，现在改成渐变了，用到下面的一个函数了
TEXT_COLOR      = (230, 225, 245)
TEXT_DIM        = (160, 155, 180)
HP_COLOR        = (100, 220, 120)
EXP_COLOR       = (100, 180, 255)
ENEMY_COLOR     = (255, 120, 120)
GAMEOVER_COLOR  = (255, 80, 80)
STORY_COLOR     = (255, 200, 230)

STATE_MENU   = "menu"
STATE_PANEL  = "panel"
STATE_BATTLE = "battle"
STATE_OVER   = "over"


# ===========================================================================
# 中文字体加载
# 策略：优先直接读 Windows 系统字体文件（最稳定，打包后也可用），
#       然后尝试打包内置字体，最后用 SysFont 回退。
# ===========================================================================
def _try_font(font_path, size):
    """尝试加载字体并验证能否渲染中文，成功返回字体对象，失败返回 None"""
    try:
        if not os.path.exists(font_path):
            return None
        f = pygame.font.Font(font_path, size)
        test = f.render('\u6d4b\u8bd5123', True, (255, 255, 255))
        if test.get_width() > 10:
            return f
    except Exception:
        pass
    return None


def load_chinese_font(size):
    # ---- 第1优先：Windows 系统字体文件（直接读路径，最稳定） ----
    system_fonts = [
        r'C:\Windows\Fonts\msyh.ttc',       # 微软雅黑（Win10/11默认）
        r'C:\Windows\Fonts\msyhbd.ttc',     # 微软雅黑粗体
        r'C:\Windows\Fonts\simhei.ttf',     # 黑体
        r'C:\Windows\Fonts\simsun.ttc',     # 宋体
        r'C:\Windows\Fonts\dengxian.ttf',   # 等线
        r'C:\Windows\Fonts\Deng.ttf',
        r'C:\Windows\Fonts\Dengb.ttf',
    ]
    for fp in system_fonts:
        f = _try_font(fp, size)
        if f:
            return f

    # ---- 第2优先：打包内置字体（如果存在） ----
    builtin_candidates = [
        resource_path(os.path.join('fonts', 'DroidSansFallbackFull.ttf')),
        resource_path('DroidSansFallbackFull.ttf'),
    ]
    if getattr(sys, 'frozen', False):
        builtin_candidates.append(
            os.path.join(os.path.dirname(sys.executable), 'fonts', 'DroidSansFallbackFull.ttf'))
        builtin_candidates.append(
            os.path.join(os.path.dirname(sys.executable), 'DroidSansFallbackFull.ttf'))
    for fp in builtin_candidates:
        f = _try_font(fp, size)
        if f:
            return f

    # ---- 第3优先：pygame SysFont（按名称查找） ----
    for name in ['microsoftyahei', 'msyh', 'simhei', 'simsun', 'dengxian',
                 'notosanscjksc', 'wenquanyimicrohei', 'arialunicode']:
        try:
            f = pygame.font.SysFont(name, size)
            test = f.render('\u6d4b\u8bd5123', True, (255, 255, 255))
            if test.get_width() > 10:
                return f
        except Exception:
            continue

    # ---- 最后回退：默认字体（至少保证数字能显示） ----
    try:
        return pygame.font.Font(None, size)
    except Exception:
        return pygame.font.SysFont(None, size)


# ===========================================================================
# 按钮类
# ===========================================================================
class Button:
    def __init__(self, rect, text, font, on_click, enabled=True, color=None):
        self.rect = pygame.Rect(rect)
        self.text = text
        self.font = font
        self.on_click = on_click
        self.enabled = enabled
        self.hover = False
        self.pressed = False
        self.color = color

    def draw(self, surface):
        if not self.enabled:
            bg, tc = BTN_DISABLED, BTN_DIS_TEXT
        elif self.pressed:
            bg, tc = (self.color if self.color else BTN_PRESS), BTN_TEXT
        elif self.hover:
            bg, tc = (self.color if self.color else BTN_HOVER), BTN_TEXT
        else:
            bg, tc = (self.color if self.color else BTN_NORMAL), BTN_TEXT
        pygame.draw.rect(surface, bg, self.rect, border_radius=8)
        pygame.draw.rect(surface, PANEL_BORDER, self.rect, 2, border_radius=8)
        ts = self.font.render(self.text, True, tc)
        surface.blit(ts, ts.get_rect(center=self.rect.center))

    def handle_event(self, event):
        if not self.enabled:
            return
        if event.type == pygame.MOUSEMOTION:
            self.hover = self.rect.collidepoint(event.pos)
        elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            if self.rect.collidepoint(event.pos):
                self.pressed = True
        elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
            if self.pressed and self.rect.collidepoint(event.pos):
                self.on_click()
            self.pressed = False


# ===========================================================================
# 消息日志
# ===========================================================================
class MessageLog:
    def __init__(self, rect, font):
        self.rect = pygame.Rect(rect)
        self.font = font
        self.lines = []
        self.max_lines = 500
        self.scroll_offset = 0

    def add(self, text, color=TEXT_COLOR):
        for segment in str(text).split("\n"):
            self._wrap_and_add(segment, color)

    def _wrap_and_add(self, text, color):
        if not text:
            self.lines.append(("", color))
            return
        max_w = self.rect.width - 24
        current = ""
        for ch in text:
            test = current + ch
            if self.font.size(test)[0] > max_w and current:
                self.lines.append((current, color))
                current = ch
            else:
                current = test
        if current:
            self.lines.append((current, color))
        if len(self.lines) > self.max_lines:
            self.lines = self.lines[-self.max_lines:]
        self.scroll_offset = 0

    def draw(self, surface):
        pygame.draw.rect(surface, LOG_BG, self.rect, border_radius=6)
        pygame.draw.rect(surface, LOG_BORDER, self.rect, 2, border_radius=6)
        clip = surface.get_clip()
        surface.set_clip(self.rect.inflate(-8, -8))
        line_h = self.font.get_linesize()
        visible = self.rect.inflate(-8, -8).height // line_h
        total = len(self.lines)
        start = max(0, total - visible - self.scroll_offset)
        end = min(total, start + visible + 1)
        y = self.rect.y + 6
        for i in range(start, end):
            text, color = self.lines[i]
            if text:
                surface.blit(self.font.render(text, True, color), (self.rect.x + 10, y))
            y += line_h
        surface.set_clip(clip)

    def scroll(self, delta):
        self.scroll_offset = max(0, self.scroll_offset + delta)


# ===========================================================================
# 进度条
# ===========================================================================
def draw_bar(surface, rect, ratio, fg_color, bg_color=(50, 48, 65), border=True):
    rect = pygame.Rect(rect)
    ratio = max(0.0, min(1.0, ratio))
    pygame.draw.rect(surface, bg_color, rect, border_radius=4)
    if ratio > 0:
        fill = rect.copy()
        fill.width = int(rect.width * ratio)
        pygame.draw.rect(surface, fg_color, fill, border_radius=4)
    if border:
        pygame.draw.rect(surface, PANEL_BORDER, rect, 1, border_radius=4)

#颜色渐变函数
def render_gradient_text(font, text, color_left, color_right):
    # """渲染水平渐变颜色的文字，返回一个 pygame.Surface"""
    # # 1. 先用白色渲染文字，得到文字形状（alpha通道）
    text_surf = font.render(text, True, (255, 255, 255))
    w, h = text_surf.get_size()
    # 2. 创建同样大小的水平渐变 surface
    gradient = pygame.Surface((w, h-5), pygame.SRCALPHA)
    for px in range(w):
        ratio = px / max(w - 1, 1)
        r = int(color_left[0] + (color_right[0] - color_left[0]) * ratio)
        g = int(color_left[1] + (color_right[1] - color_left[1]) * ratio)
        b = int(color_left[2] + (color_right[2] - color_left[2]) * ratio)
        pygame.draw.line(gradient, (r, g, b), (px, 0), (px, h))
    # 3. 用文字的 alpha 通道作为 mask，把渐变乘到文字上
    gradient.blit(text_surf, (0, 0), special_flags=pygame.BLEND_RGBA_MULT)
    return gradient

# ===========================================================================
# 主游戏类
# ===========================================================================
class Game:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WIN_W, WIN_H))
        pygame.display.set_caption("\u6a79\u7a46\u4fee\u4ed9\u4f20 \u2014 C++\u903b\u8f91 + Python\u754c\u9762")
        self.clock = pygame.time.Clock()

        self.font_title = load_chinese_font(30)
        self.font_h2    = load_chinese_font(20)
        self.font       = load_chinese_font(17)
        self.font_small = load_chinese_font(14)
        self.font_btn   = load_chinese_font(18)

        self.session = gamelogic.GameSession()
        self.state = STATE_MENU

        self.log = MessageLog((330, 60, 655, 470), self.font)
        for line in self.session.get_intro():
            if "\u5267\u60c5" in line or line.startswith("===="):
                self.log.add(line, STORY_COLOR)
            else:
                self.log.add(line)

        self.buttons = []
        self._build_buttons()

    def _build_buttons(self):
        self.buttons = []
        bx, by, bw, bh, gap = 340, 550, 140, 44, 16

        if self.state == STATE_MENU:
            items = [
                ("1 面板", self._open_panel),
                ("2 休息", self._do_xiuxi),
                ("3 修炼", self._do_xiulian),
                ("4 游历", self._do_youli),
                ("9 退出", self._quit),
            ]
            for i, (txt, cb) in enumerate(items):
                self.buttons.append(Button((bx + i * (bw + gap), by, bw, bh), txt, self.font_btn, cb))

        elif self.state == STATE_PANEL:
            items = [
                ("a 体质+1", lambda: self._add_point('a')),
                ("s 敏捷+1", lambda: self._add_point('s')),
                ("d 悟性+1", lambda: self._add_point('d')),
                ("f 精神+1", lambda: self._add_point('f')),
                ("z 返回", self._back_to_menu),
            ]
            for i, (txt, cb) in enumerate(items):
                self.buttons.append(Button((bx + i * (bw + gap), by, bw, bh), txt, self.font_btn, cb))

        elif self.state == STATE_BATTLE:
            if self.session.battle_pending_choice:
                items = [("a 开始战斗", self._battle_start), ("s 撤退", self._battle_flee)]
            else:
                items = [("a 战斗", self._battle_attack), ("s 撤退", self._battle_flee)]
            for i, (txt, cb) in enumerate(items):
                self.buttons.append(Button((bx + i * (bw + gap) + 80, by, bw + 20, bh), txt, self.font_btn, cb))

        elif self.state == STATE_OVER:
            self.buttons.append(Button((bx + 200, by, 200, bh), "\u91cd\u65b0\u5f00\u59cb", self.font_btn, self._restart))
            self.buttons.append(Button((bx + 430, by, 140, bh), "\u9000\u51fa", self.font_btn, self._quit))

    def _push_msgs(self, msgs):
        for m in msgs:
            if "\u5267\u60c5" in m or m.startswith("====") or m.startswith("====="):
                self.log.add(m, STORY_COLOR)
            elif "\u80dc\u5229" in m:
                self.log.add(m, HP_COLOR)
            elif "\u5931\u8d25" in m or "\u635f\u5931" in m or "\u51cf\u5c11" in m or "\u4e0d\u8db3" in m:
                self.log.add(m, ENEMY_COLOR)
            elif "\u5347\u7ea7" in m or "\u589e\u52a0" in m or "\u63d0\u9ad8" in m or "\u56de\u590d" in m:
                self.log.add(m, HP_COLOR)
            else:
                self.log.add(m)
        self._check_state()

    def _check_state(self):
        if self.session.game_over:
            self.state = STATE_OVER
            self.log.add(self.session.game_over_msg, GAMEOVER_COLOR)
        elif self.session.in_battle or self.session.battle_pending_choice:
            self.state = STATE_BATTLE
        else:
            if self.state != STATE_PANEL:
                self.state = STATE_MENU
        self._build_buttons()

    def _open_panel(self):
        self.state = STATE_PANEL
        for line in self.session.get_panel_info():
            self.log.add(line, TITLE_COLOR)
        self._build_buttons()

    def _back_to_menu(self):
        self.state = STATE_MENU
        self.log.add("\u8fd4\u56de\u4e3b\u83dc\u5355", TEXT_DIM)
        self._build_buttons()

    def _do_xiuxi(self):    self._push_msgs(self.session.action_xiuxi())
    def _do_xiulian(self):  self._push_msgs(self.session.action_xiulian())
    def _do_youli(self):    self._push_msgs(self.session.action_youli())
    def _add_point(self, a): self._push_msgs(self.session.add_point(a))
    def _battle_start(self): self._push_msgs(self.session.battle_start())
    def _battle_attack(self): self._push_msgs(self.session.battle_attack())
    def _battle_flee(self):  self._push_msgs(self.session.battle_flee())

    def _quit(self):
        pygame.quit()
        sys.exit(0)

    def _restart(self):
        self.session = gamelogic.GameSession()
        self.state = STATE_MENU
        self.log = MessageLog((330, 60, 655, 470), self.font)
        for line in self.session.get_intro():
            self.log.add(line)
        self._build_buttons()

    def handle_key(self, key):
        if self.state == STATE_OVER:
            if key == pygame.K_r:
                self._restart()
            return
        if self.state == STATE_MENU:
            {pygame.K_1: self._open_panel, pygame.K_2: self._do_xiuxi,
             pygame.K_3: self._do_xiulian, pygame.K_4: self._do_youli,
             pygame.K_9: self._quit}.get(key, lambda: None)()
        elif self.state == STATE_PANEL:
            if key == pygame.K_a: self._add_point('a')
            elif key == pygame.K_s: self._add_point('s')
            elif key == pygame.K_d: self._add_point('d')
            elif key == pygame.K_f: self._add_point('f')
            elif key == pygame.K_z: self._back_to_menu()
        elif self.state == STATE_BATTLE:
            if key == pygame.K_a:
                if self.session.battle_pending_choice:
                    self._battle_start()
                else:
                    self._battle_attack()
            elif key == pygame.K_s:
                self._battle_flee()

    def draw_player_panel(self):
        panel_rect = pygame.Rect(15, 60, 305, 580)
        pygame.draw.rect(self.screen, PANEL_BG, panel_rect, border_radius=10)
        pygame.draw.rect(self.screen, PANEL_BORDER, panel_rect, 2, border_radius=10)

        p, s = self.session.wanglujie, self.session
        x, y = panel_rect.x + 16, panel_rect.y + 14

        name_surf = render_gradient_text(
        self.font_title, "王橹杰",
        (135, 206, 235),   # #87CEEB 天蓝色（左）
        (255, 192, 203)    # #FFC0CB 粉红色（右）
        )
        self.screen.blit(name_surf, (x, y))
        y += 40
        self.screen.blit(self.font_h2.render(f"第 {s.t} 天", True, TEXT_DIM), (x, y))
        y += 30
        self.screen.blit(self.font.render(f"等级  {p.dengji} / {s.maxdengji}", True, TEXT_COLOR), (x, y))
        y += 24

        exp_ratio = p.jingyan / s.maxjingyan if s.maxjingyan > 0 else 0
        draw_bar(self.screen, (x, y, 273, 14), exp_ratio, EXP_COLOR)
        self.screen.blit(self.font_small.render(f"经验 {p.jingyan:.1f} / {s.maxjingyan:.0f}", True, TEXT_DIM), (x, y + 16))
        y += 38

        hp_ratio = p.xueliang / s.maxxueliang if s.maxxueliang > 0 else 0
        draw_bar(self.screen, (x, y, 273, 14), hp_ratio, HP_COLOR)
        self.screen.blit(self.font_small.render(f"血量 {p.xueliang:.1f} / {s.maxxueliang:.0f}", True, TEXT_DIM), (x, y + 16))
        y += 38

        pygame.draw.line(self.screen, PANEL_BORDER, (x, y), (x + 273, y), 1)
        y += 12

        for name, val, col in [
            ("攻击力：", p.gongji, TEXT_COLOR),
            ("体质：", p.tizhi, TEXT_COLOR),
            ("敏捷：", p.mingjie, TEXT_COLOR),
            ("悟性：", p.wuxing, TEXT_COLOR),
            ("精神：", f"{p.jingshen:.1f}", TEXT_COLOR),
            ("自由属性点：", p.shuxingdian, TITLE_COLOR),
            ("总击杀数：", s.kill, TEXT_DIM),
            ("总经验值：", f"{s.zongjingyan:.1f}", TEXT_DIM),
        ]:
            self.screen.blit(self.font.render(f"{name}\uff1a{val}", True, col), (x, y))
            y += 24

        y += 6
        pygame.draw.line(self.screen, PANEL_BORDER, (x, y), (x + 273, y), 1)
        y += 10
        self.screen.blit(self.font_small.render(
            f"成就  I DONT CARE: {s.lm1}", True, STORY_COLOR), (x, y))
        y += 18
        self.screen.blit(self.font_small.render(
            f"         落叶知我意，天公协美助: {s.lm2}", True, STORY_COLOR), (x, y))
        y += 18
        self.screen.blit(self.font_small.render(
            f"         爱夫者风生水起: {s.lm3}", True, STORY_COLOR), (x, y))
        y += 18
        self.screen.blit(self.font_small.render(
            f"         世纪婚礼: {s.lm4}", True, STORY_COLOR), (x, y))
        if s.in_battle or s.battle_pending_choice:
            e = s.current_enemy 
            ey = panel_rect.bottom - 130
            ebg = pygame.Rect(x + 730, ey + 30, 273, 116)
            pygame.draw.rect(self.screen, (55, 30, 35), ebg, border_radius=8)
            pygame.draw.rect(self.screen, ENEMY_COLOR, ebg, 2, border_radius=8)
            self.screen.blit(self.font_h2.render(f"敌人：{e.name}", True, ENEMY_COLOR), (x + 740, ey + 38))
            self.screen.blit(self.font_small.render(f"等级： {e.dengji}  攻击力： {e.gongji}", True, TEXT_DIM), (x + 740, ey + 62))
            ehp = e.xueliang / e.maxxl if e.maxxl > 0 else 0
            draw_bar(self.screen, (x + 740, ey + 84, 253, 12), ehp, ENEMY_COLOR)
            self.screen.blit(self.font_small.render(f"血量： {e.xueliang:.1f} / {e.maxxl:.1f}", True, TEXT_DIM), (x + 740, ey + 100))
            if s.battle_pending_choice:
                self.screen.blit(self.font_small.render("快捷键 a 战斗 / s 撤退", True, TITLE_COLOR), (x + 740, ey + 120))

    def draw_top_bar(self):
        bar = pygame.Rect(0, 0, WIN_W, 50)
        pygame.draw.rect(self.screen, (30, 26, 50), bar)
        self.screen.blit(self.font_title.render("   橹韵祉朝", True, TITLE_COLOR), (20, 10))
        state_text = {STATE_MENU: "虚间", STATE_PANEL: "属性面板",
                      STATE_BATTLE: "战斗中", STATE_OVER: "\u6e38\u620f\u7ed3\u675f"}.get(self.state, "")
        self.screen.blit(self.font_h2.render(f"[{state_text}]", True, TEXT_DIM), (WIN_W - 100, 14))

    def draw_hint(self):
        hints = {
            STATE_MENU:   "1 面板 2 休息 3 修炼 4 游历 9 退出游戏",
            STATE_PANEL:  "a 体质 s 敏捷 d 悟性(3点) f 精神 z 返回",
            STATE_BATTLE: "a 攻击 s 逃跑",
            STATE_OVER:   "\u6309 R \u91cd\u65b0\u5f00\u59cb\uff0c\u6216\u70b9\u51fb\u6309\u94ae",
        }
        self.screen.blit(self.font_small.render(hints.get(self.state, ""), True, TEXT_DIM), (340, 610))

    def draw_game_over(self):
        overlay = pygame.Surface((WIN_W, WIN_H), pygame.SRCALPHA)
        overlay.fill((0, 0, 0, 140))
        self.screen.blit(overlay, (0, 0))
        txt = self.font_title.render("\u2014 \u6e38\u620f\u7ed3\u675f \u2014", True, GAMEOVER_COLOR)
        self.screen.blit(txt, txt.get_rect(center=(WIN_W // 2, 300)))

    def run(self):
        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN:
                    self.handle_key(event.key)
                elif event.type == pygame.MOUSEWHEEL:
                    self.log.scroll(-event.y * 3)
                for btn in self.buttons:
                    btn.handle_event(event)

            self.screen.fill(BG_COLOR)
            self.draw_top_bar()
            self.draw_player_panel()
            self.log.draw(self.screen)
            self.draw_hint()
            for btn in self.buttons:
                btn.draw(self.screen)
            if self.state == STATE_OVER:
                self.draw_game_over()

            pygame.display.flip()
            self.clock.tick(FPS)
        pygame.quit()


if __name__ == "__main__":
    game = Game()
    game.run()
#（注：内容由AI生成,豆包个蠢猪）
