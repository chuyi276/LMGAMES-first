# -*- mode: python ; coding: utf-8 -*-
import os
import sys
from PyInstaller.utils.hooks import collect_all

block_cipher = None
pygame_datas, pygame_binaries, pygame_hiddenimports = collect_all('pygame')
project_root = os.path.abspath('.')

a = Analysis(
    ['game_ui.py'],
    pathex=[project_root],
    binaries=pygame_binaries,
    datas=pygame_datas + [
        (os.path.join(project_root, 'fonts', 'DroidSansFallbackFull.ttf'), 'fonts'),
    ],
    hiddenimports=pygame_hiddenimports + ['gamelogic'],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['tkinter', 'matplotlib', 'numpy', 'scipy', 'pandas', 'PIL', 'cv2', 'torch', 'tensorflow'],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='LMG_Game',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

coll = COLLECT(
    exe,
    a.binaries,
    a.zipfiles,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='LMG_Game',
)
