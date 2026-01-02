from building import *
import os

cwd = GetCurrentDir()
path = [os.path.join(cwd, 'inc')]
src_path = os.path.join(cwd, 'src')

CPPDEFINES = ['USE_STDPERIPH_DRIVER']

src = [
os.path.join(src_path, 'at32f403a_407_acc.c'),
os.path.join(src_path, 'at32f403a_407_adc.c'),
os.path.join(src_path, 'at32f403a_407_bpr.c'),
os.path.join(src_path, 'at32f403a_407_can.c'),
os.path.join(src_path, 'at32f403a_407_crc.c'),
os.path.join(src_path, 'at32f403a_407_crm.c'),
os.path.join(src_path, 'at32f403a_407_dac.c'),
os.path.join(src_path, 'at32f403a_407_debug.c'),
os.path.join(src_path, 'at32f403a_407_dma.c'),
os.path.join(src_path, 'at32f403a_407_emac.c'),
os.path.join(src_path, 'at32f403a_407_exint.c'),
os.path.join(src_path, 'at32f403a_407_flash.c'),
os.path.join(src_path, 'at32f403a_407_gpio.c'),
os.path.join(src_path, 'at32f403a_407_i2c.c'),
os.path.join(src_path, 'at32f403a_407_misc.c'),
os.path.join(src_path, 'at32f403a_407_pwc.c'),
os.path.join(src_path, 'at32f403a_407_rtc.c'),
os.path.join(src_path, 'at32f403a_407_sdio.c'),
os.path.join(src_path, 'at32f403a_407_spi.c'),
os.path.join(src_path, 'at32f403a_407_tmr.c'),
os.path.join(src_path, 'at32f403a_407_usart.c'),
os.path.join(src_path, 'at32f403a_407_usb.c'),
os.path.join(src_path, 'at32f403a_407_wdt.c'),
os.path.join(src_path, 'at32f403a_407_wwdt.c'),
os.path.join(src_path, 'at32f403a_407_xmc.c'),
]

group = DefineGroup('libraries', src, depend = ['PKG_USING_AT32F403A_407_HAL_DRIVER'], CPPPATH = path, CPPDEFINES = CPPDEFINES)

Return('group')
