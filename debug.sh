#/bin/bash
date >> logs/debuglog.log
echo "" >> logs/debuglog.log
"/mnt/c/Program Files/Oracle/VirtualBox/VBoxManage.exe" startvm "Clement" -E VBOX_GUI_DBG_AUTO_SHOW=true -E VBOX_GUI_DBG_ENABLED=true

echo "Debug Complete"
