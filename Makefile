# 总的 Makefile，用于调用目录下各个子工程对应的 Makefile
# 注意： Linux 下编译方式：
# 1. 从 http://pkgman.jieliapp.com/doc/all 处找到下载链接
# 2. 下载后，解压到 /opt/jieli 目录下，保证
#   /opt/jieli/common/bin/clang 存在（注意目录层次）
# 3. 确认 ulimit -n 的结果足够大（建议大于8096），否则链接可能会因为打开文件太多而失败
#   可以通过 ulimit -n 8096 来设置一个较大的值
# 支持的目标
# make ac57xx_hunting_camera
# make ac57xx_car_camera
# make ac57xx_ipc_camera
# make ac57xx_ipc_doorbell
# make ac57xx_wifi_car_camera

.PHONY: all clean ac57xx_hunting_camera ac57xx_car_camera ac57xx_ipc_camera ac57xx_ipc_doorbell ac57xx_wifi_car_camera clean_ac57xx_hunting_camera clean_ac57xx_car_camera clean_ac57xx_ipc_camera clean_ac57xx_ipc_doorbell clean_ac57xx_wifi_car_camera

all: ac57xx_hunting_camera ac57xx_car_camera ac57xx_ipc_camera ac57xx_ipc_doorbell ac57xx_wifi_car_camera
	@echo +ALL DONE

clean: clean_ac57xx_hunting_camera clean_ac57xx_car_camera clean_ac57xx_ipc_camera clean_ac57xx_ipc_doorbell clean_ac57xx_wifi_car_camera
	@echo +CLEAN DONE

ac57xx_hunting_camera:
	$(MAKE) -C . -f Makefile.ac57xx_hunting_camera

clean_ac57xx_hunting_camera:
	$(MAKE) -C . -f Makefile.ac57xx_hunting_camera clean

ac57xx_car_camera:
	$(MAKE) -C . -f Makefile.ac57xx_car_camera

clean_ac57xx_car_camera:
	$(MAKE) -C . -f Makefile.ac57xx_car_camera clean

ac57xx_ipc_camera:
	$(MAKE) -C . -f Makefile.ac57xx_ipc_camera

clean_ac57xx_ipc_camera:
	$(MAKE) -C . -f Makefile.ac57xx_ipc_camera clean

ac57xx_ipc_doorbell:
	$(MAKE) -C . -f Makefile.ac57xx_ipc_doorbell

clean_ac57xx_ipc_doorbell:
	$(MAKE) -C . -f Makefile.ac57xx_ipc_doorbell clean

ac57xx_wifi_car_camera:
	$(MAKE) -C . -f Makefile.ac57xx_wifi_car_camera

clean_ac57xx_wifi_car_camera:
	$(MAKE) -C . -f Makefile.ac57xx_wifi_car_camera clean
