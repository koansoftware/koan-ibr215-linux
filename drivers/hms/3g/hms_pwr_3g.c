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
  
#define DRIVER_NAME "hms_pwr_3g"  

static const struct of_device_id hms_3g_dt_ids[] = {  
    { .compatible = "hms_pwr_3g", },  
    { /* sentinel */ }  
};  
MODULE_DEVICE_TABLE(of, hms_3g_dt_ids);  

static int hms_pwr_3g_probe(struct platform_device *pdev)  
{  
    struct device_node *chip = NULL;  
    int chip_power_gpio, chip_wake_gpio,chip_req_gpio,chip_dis_gpio,chip_en_gpio,chip_reset_gpio;  
	int ret;
  
    printk("........%s\n",__func__);  
  
    chip = pdev->dev.of_node;  

    chip_power_gpio = of_get_named_gpio(chip, "chip_power_gpio", 0);  
    if (gpio_is_valid(chip_power_gpio)) {  
        ret = devm_gpio_request_one(&pdev->dev,chip_power_gpio, GPIOF_OUT_INIT_HIGH,  
            "chip_power_gpio");  
        if (ret) 
            pr_warn("failed to request chip_power_gpio\n");  

	}

    chip_wake_gpio = of_get_named_gpio(chip, "chip_wake_gpio", 0);  
    if (gpio_is_valid(chip_wake_gpio)) {  
        ret = devm_gpio_request_one(&pdev->dev,chip_wake_gpio, GPIOF_DIR_IN,  
            "chip_wake_gpio");  
#if 0
        if (ret)  
            pr_warn("failed to request chip_wake_gpio\n");  
		else
			gpio_set_value(chip_wake_gpio, 0);
#endif
    }  

    chip_req_gpio = of_get_named_gpio(chip, "chip_req_gpio", 0);  
    if (gpio_is_valid(chip_req_gpio)) {  
        //ret = devm_gpio_request_one(&pdev->dev,chip_req_gpio, GPIOF_OUT_INIT_LOW,  
        ret = devm_gpio_request_one(&pdev->dev,chip_req_gpio, GPIOF_DIR_IN,  
            "chip_req_gpio");  
        if (ret)  
            pr_warn("failed to request chip_req_gpio\n");  
    }  

    chip_dis_gpio = of_get_named_gpio(chip, "chip_dis_gpio", 0);  
    if (gpio_is_valid(chip_dis_gpio)) {  
        ret = devm_gpio_request_one(&pdev->dev,chip_dis_gpio, GPIOF_OUT_INIT_HIGH,  
            "chip_dis_gpio");  
        if (ret)  
            pr_warn("failed to request chip_dis_gpio\n");  
    }  

	
    chip_en_gpio = of_get_named_gpio(chip, "chip_en_gpio", 0);  
    if (gpio_is_valid(chip_en_gpio)) {  
        ret = devm_gpio_request_one(&pdev->dev,chip_en_gpio, GPIOF_OUT_INIT_HIGH,  
            "chip_en_gpio");  
        if (ret)  
            pr_warn("failed to request chip_en_gpio\n");  
		else
			gpio_set_value(chip_en_gpio, 1);
    }  

    chip_reset_gpio = of_get_named_gpio(chip, "chip_reset_gpio", 0);  
    if (gpio_is_valid(chip_reset_gpio)) {  
        ret = devm_gpio_request_one(&pdev->dev,chip_reset_gpio, GPIOF_OUT_INIT_LOW,  
            "chip_power_gpio");  
        if (ret) {
            pr_warn("failed to request chip_reset_gpio\n");  
		}else{
			gpio_set_value_cansleep(chip_reset_gpio, 0);
			mdelay(200);
			gpio_set_value_cansleep(chip_reset_gpio, 1);
			mdelay(200);
			//printk("\n%s:chip_reset_gpio--------\n",__func__);
		}
    }  
 
    return ret;  
}  
  
static struct platform_driver hms_3g_driver = {  
    .probe  = hms_pwr_3g_probe,  
    .driver = {  
        .of_match_table = hms_3g_dt_ids,  
        .name   = DRIVER_NAME,  
        .owner  = THIS_MODULE,  
    },  
};  
//module_platform_driver(hms_3g_driver);  

static int __init hms_3g_drv_init(void)
{
	return platform_driver_register(&hms_3g_driver);
}

late_initcall(hms_3g_drv_init);


MODULE_AUTHOR("HMS");  
MODULE_DESCRIPTION("hms 3g gpio driver");  
MODULE_LICENSE("GPL");  
MODULE_ALIAS("platform:" DRIVER_NAME);  
