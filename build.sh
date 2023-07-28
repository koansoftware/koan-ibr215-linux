source /opt/fsl-imx-wayland/5.4-zeus/environment-setup-aarch64-poky-linux 

make imx_v8_ibr215_defconfig

#make freescale/imx8mp-evk.dtb
make freescale/imx8mp-evk-ibr215.dtb
make freescale/imx8mp-evk-ibr215-hdmi.dtb
make freescale/imx8mp-evk-ibr215-lvds.dtb
#make freescale/imx8mp-evk-lm215wf3-lvds-panel.dtb
#make freescale/imx8mp-evk-lm215wf3-lvds-panel-hdmi.dtb
#
#make freescale/imx8mp-evk-hx100-smc.dtb
#make freescale/imx8mp-evk-tvmb.dtb
#make freescale/imx8mp-evk-tvmb-lm215wf3-lvds-panel.dtb
#make freescale/imx8mp-evk-tvmb-ptjmntlcd050001-lvds-panel.dtb
#make freescale/imx8mp-evk-tvmb-jwz035p1erc6v0-rgb-panel.dtb
#make freescale/imx8mp-evk-lm215wf3-lvds-panel-hx100-smc.dtb
#make freescale/imx8mp-evk-lm215wf3-lvds-panel-hdmi-hx100-smc.dtb
#
#make freescale/imx8mp-evk-rp103-smc.dtb
#make freescale/imx8mp-evk-lm215wf3-lvds-panel-rp103-smc.dtb
#make freescale/imx8mp-evk-lm215wf3-lvds-panel-hdmi-rp103-smc.dtb
#

make Image -j8

#make modules -j8
#make modules_install INSTALL_MOD_PATH=`pwd`

#cp arch/arm64/boot/dts/freescale/imx8mp-evk.dtb /home/share/jian/
#cp arch/arm64/boot/dts/freescale/imx8mp-evk-lm215wf3-lvds-panel.dtb /home/share/jian/imx8mp-evk.dtb
#cp arch/arm64/boot/Image /home/share/jian/

