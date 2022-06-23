openocd -f toolchain/openocd_discovery.cfg -c init -c "reset halt" -c "flash write_image erase build/"$1 -c "verify_image build/"$1 -c reset -c shutdown 
