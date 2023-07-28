#include <linux/device.h>  
#include <linux/err.h>  
#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/of.h>  
#include <linux/of_gpio.h>  
#include <linux/platform_device.h>  
#include <linux/regulator/consumer.h>  
#include <linux/gpio.h>  
#include <linux/delay.h>
  
#define DRIVER_NAME "hms_gpio_wifi"  

int wifi_led_gpio;  

static const struct of_device_id wifi_led_dt_ids[] = {  
    { .compatible = "hms_gpio_wifi", },  
    { /* sentinel */ }  
};  
MODULE_DEVICE_TABLE(of, wifi_led_dt_ids);  

static int chip_probe(struct platform_device *pdev)  
{  
    struct device_node *chip = NULL;  
	int ret=0;
  
    printk("%s\n",__func__);  
  
    chip = pdev->dev.of_node;  

    wifi_led_gpio = of_get_named_gpio(chip, "led_gpio", 0);  
    if (gpio_is_valid(wifi_led_gpio)) {  
        ret = devm_gpio_request_one(&pdev->dev,wifi_led_gpio, GPIOF_OUT_INIT_LOW,  
            "led_gpio");  
        if (ret) 
            pr_warn("failed to request wifi_led_gpio\n");  
	}

    return ret;  
}  

void wifi_led_control(int onoff)
{
    gpio_set_value(wifi_led_gpio, onoff);
};

EXPORT_SYMBOL(wifi_led_control);
  
static struct platform_driver wifi_led_driver = {  
    .probe  = chip_probe,  
    .driver = {  
        .of_match_table = wifi_led_dt_ids,  
        .name   = DRIVER_NAME,  
        .owner  = THIS_MODULE,  
    },  
};  
//module_platform_driver(wifi_led_driver);  
  
static int __init hms_wifi_drv_init(void)
{
	return platform_driver_register(&wifi_led_driver);
}

late_initcall(hms_wifi_drv_init);

MODULE_AUTHOR("HMS");  
MODULE_DESCRIPTION("hms wifi gpio driver");  
MODULE_LICENSE("GPL");  
MODULE_ALIAS("platform:" DRIVER_NAME);  
