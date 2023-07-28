/*
 * Copyright (C) 2013-2015 Freescale Semiconductor, Inc.
 *
 * Based on imx-sgtl5000.c
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 * Copyright (C) 2012 Linaro Ltd.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/i2c.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#ifdef CONFIG_SWITCH
#include <linux/switch.h>
#endif
#include <sound/soc.h>
#include <sound/jack.h>
#include <sound/control.h>
#include <sound/pcm_params.h>
#include <sound/soc-dapm.h>
#include <linux/pinctrl/consumer.h>
#include "imx-ssi.h"

#include "../codecs/rt5640.h"
#include "imx-audmux.h"

#define DAI_NAME_SIZE	32

struct imx_rt5640_data {
	struct snd_soc_dai_link *dai;
	struct snd_soc_card card;
	char codec_dai_name[DAI_NAME_SIZE];
	char platform_name[DAI_NAME_SIZE];
	unsigned int clk_frequency;
};

struct imx_priv {
    struct snd_soc_codec *codec;
	struct platform_device *pdev;
    struct clk *codec_clk;
};
static struct imx_priv card_priv;

static int sample_rate = 44100;
static snd_pcm_format_t sample_format = SNDRV_PCM_FORMAT_S16_LE;


static const struct snd_soc_dapm_widget imx_rt5640_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Line Out Jack", NULL),
	SND_SOC_DAPM_MIC("Line In Jack", NULL),
#if 0
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_SPK("Ext Spk", NULL),
	SND_SOC_DAPM_MIC("AMIC", NULL),
	SND_SOC_DAPM_MIC("DMIC", NULL),
#endif
};

static int imx_hifi_hw_params(struct snd_pcm_substream *substream,
			struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	//struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *codec_dai = asoc_rtd_to_codec(rtd, 0);
	//struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *cpu_dai = asoc_rtd_to_cpu(rtd, 0);
	struct imx_priv *priv = &card_priv;
	struct device *dev = &priv->pdev->dev;
	struct snd_soc_card *card = platform_get_drvdata(priv->pdev);
	struct imx_rt5640_data *data = snd_soc_card_get_drvdata(card);
	u32 dai_format;
	int ret = 0;

	sample_rate = params_rate(params);
	sample_format = params_format(params);

    printk("function:%s line:%d params_rate(params):%d \n",__FUNCTION__,__LINE__,params_rate(params));
    printk("sample_rate:%d sample_format:%d \n",sample_rate, sample_format);
	dai_format = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_CBM_CFM;

	unsigned int fmt = SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_CBM_CFM;


	ret = snd_soc_dai_set_fmt(cpu_dai, fmt);
	if (ret) {
		dev_err(dev, "failed to set cpu dai fmt: %d\n", ret);
		return ret;
	}
	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, fmt);
	if (ret) {
		dev_err(dev, "failed to set codec dai fmt: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dai_set_tdm_slot(cpu_dai, 0, 0, 2,
					params_physical_width(params));
	if (ret) {
		dev_err(dev, "failed to set cpu dai tdm slot: %d\n", ret);
		return ret;
	}
	//ret = snd_soc_dai_set_sysclk(cpu_dai, 0, 0, SND_SOC_CLOCK_OUT);


#if 0
    /* set imx6 ssi as input */
    snd_soc_dai_set_sysclk(cpu_dai, IMX_SSP_SYS_CLK, 0, SND_SOC_CLOCK_IN);
#endif
#if 1
    printk("function:%s line:%d priv->clk_frequency :%d \n",__FUNCTION__,__LINE__,data->clk_frequency );
    if((24576000%sample_rate)==0)  //for 8k,16k,32k,48k
    {
        snd_soc_dai_set_pll(codec_dai, 0,0, data->clk_frequency, 24576000);
        snd_soc_dai_set_sysclk(codec_dai,1, 24576000, SND_SOC_CLOCK_IN);
    }else {
        snd_soc_dai_set_pll(codec_dai, 0,0, data->clk_frequency, 22579200);
        snd_soc_dai_set_sysclk(codec_dai, 1, 22579200, SND_SOC_CLOCK_IN);
    }

#endif

	return 0;
}

static struct snd_soc_ops imx_hifi_ops = {
	.hw_params = imx_hifi_hw_params,
};

SND_SOC_DAILINK_DEFS(hifi,
	DAILINK_COMP_ARRAY(COMP_EMPTY()),
	DAILINK_COMP_ARRAY(COMP_CODEC(NULL, "rt5640-aif1")),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

static struct snd_soc_dai_link imx_rt5640_dai[] = {
	{
		.name = "HiFi",
		.stream_name = "HiFi",
		.ops = &imx_hifi_ops,
		SND_SOC_DAILINK_REG(hifi),
	},
};



static int imx_rt5640_late_probe(struct snd_soc_card *card)
{
	struct snd_soc_pcm_runtime *rtd = list_first_entry(
		&card->rtd_list, struct snd_soc_pcm_runtime, list);
	//struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *codec_dai = asoc_rtd_to_codec(rtd, 0);
	struct imx_priv *priv = &card_priv;
	struct imx_rt5640_data *data = snd_soc_card_get_drvdata(card);
//	struct device *dev = &priv->pdev->dev;
	struct i2c_client *codec_dev;
	int ret;
#if 0
	ret = snd_soc_dai_set_sysclk(codec_dai, rt5640_SYSCLK_MCLK,
			data->clk_frequency, SND_SOC_CLOCK_IN);
	if (ret < 0)
		dev_err(dev, "failed to set sysclk in %s\n", __func__);
#endif
#if 1
	codec_dev = of_find_i2c_device_by_node(data->dai[0].codecs->of_node);
	if (!codec_dev || !codec_dev->dev.driver) {
		printk( "failed to find codec platform device\n");
		ret = -EINVAL;
        return ret;
	}

	priv->codec_clk = devm_clk_get(&codec_dev->dev, "mclk");
	if (IS_ERR(priv->codec_clk)) {
		ret = PTR_ERR(priv->codec_clk);
		printk( "failed to get codec clk: %d\n", ret);
        return ret;
	}

#endif
	data->clk_frequency = clk_get_rate(priv->codec_clk);
    printk("function:%s line:%d priv->clk_frequency :%d \n",__FUNCTION__,__LINE__,data->clk_frequency );

	ret = snd_soc_dai_set_sysclk(codec_dai, 0, data->clk_frequency,
							SND_SOC_CLOCK_IN);


	return ret;
}
static int imx_rt5640_probe(struct platform_device *pdev)
{
	//struct device_node *np = pdev->dev.of_node;
	struct device_node *cpu_np, *codec_np = NULL;
	struct platform_device *cpu_pdev;
	struct imx_priv *priv = &card_priv;
	//struct i2c_client *codec_dev;
	struct imx_rt5640_data *data;
	int ret;
	//u32 width;

	priv->pdev = pdev;

	cpu_np = of_parse_phandle(pdev->dev.of_node, "audio-cpu", 0);
	if (!cpu_np) {
		dev_err(&pdev->dev, "cpu dai phandle missing or invalid\n");
		ret = -EINVAL;
		goto fail;
	}

//audmux_bypass:
	codec_np = of_parse_phandle(pdev->dev.of_node, "audio-codec", 0);
	if (!codec_np) {
		dev_err(&pdev->dev, "phandle missing or invalid\n");
		ret = -EINVAL;
		goto fail;
	}

	cpu_pdev = of_find_device_by_node(cpu_np);
	if (!cpu_pdev) {
		dev_err(&pdev->dev, "failed to find SSI platform device\n");
		ret = -EINVAL;
		goto fail;
	}
#if 0
	codec_dev = of_find_i2c_device_by_node(codec_np);
    if (!codec_dev)printk("kkkkkkkkkkkkkkkkk fail to find i2c node \n");
	if (!codec_dev || !codec_dev->dev.driver) {
		dev_err(&pdev->dev, "failed to find codec platform device\n");
		ret = -EINVAL;
		goto fail;
	}

	priv->codec_clk = devm_clk_get(&codec_dev->dev, "mclk");
	if (IS_ERR(priv->codec_clk)) {
		ret = PTR_ERR(priv->codec_clk);
		dev_err(&pdev->dev, "failed to get codec clk: %d\n", ret);
		goto fail;
	}

	data->clk_frequency = clk_get_rate(priv->codec_clk);
    printk("function:%s line:%d priv->clk_frequency :%d \n",__FUNCTION__,__LINE__,data->clk_frequency );
#endif
    printk("function:%s line:%d \n",__FUNCTION__,__LINE__);
	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto fail;
	}
    printk("function:%s line:%d \n",__FUNCTION__,__LINE__);
    data->dai = imx_rt5640_dai;
    printk("function:%s line:%d \n",__FUNCTION__,__LINE__);
	//data->dai[0].name = "HiFi";
	//data->dai[0].stream_name = "HiFi";
	//data->dai[0].codecs->dai_name = "rt5640-aif1";
	//data->dai[0].ops = &imx_hifi_ops;
	data->dai[0].codecs->of_node = codec_np;
	data->dai[0].cpus->dai_name = dev_name(&cpu_pdev->dev);
	data->dai[0].platforms->of_node = cpu_np;
	data->dai[0].dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			    SND_SOC_DAIFMT_CBM_CFM;

	data->card.num_links = 1;

	data->card.dev = &pdev->dev;
	ret = snd_soc_of_parse_card_name(&data->card, "model");
	if (ret)
		goto fail;
	ret = snd_soc_of_parse_audio_routing(&data->card, "audio-routing");
	if (ret)
		goto fail;
	data->card.dai_link = data->dai;
	data->card.dapm_widgets = imx_rt5640_dapm_widgets;
	data->card.num_dapm_widgets = ARRAY_SIZE(imx_rt5640_dapm_widgets);

	data->card.late_probe = imx_rt5640_late_probe;

	//data->card.set_bias_level = imx_rt5640_set_bias_level;
	platform_set_drvdata(pdev, &data->card);
	snd_soc_card_set_drvdata(&data->card, data);
	ret = devm_snd_soc_register_card(&pdev->dev, &data->card);
	if (ret) {
		dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n", ret);
		goto fail;
	}
	goto fail;

//fail_hp:
fail:
	if (cpu_np)
		of_node_put(cpu_np);
	if (codec_np)
		of_node_put(codec_np);

	return ret;
}

static int imx_rt5640_remove(struct platform_device *pdev)
{
	//struct snd_soc_card *card = platform_get_drvdata(pdev);
	//struct imx_priv *priv = &card_priv;
    return 0;
}

static const struct of_device_id imx_rt5640_dt_ids[] = {
	{ .compatible = "fsl,imx-audio-rt5640", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx_rt5640_dt_ids);

static struct platform_driver imx_rt5640_driver = {
	.driver = {
		.name = "imx-rt5640",
		.owner = THIS_MODULE,
		.pm = &snd_soc_pm_ops,
		.of_match_table = imx_rt5640_dt_ids,
	},
	.probe = imx_rt5640_probe,
	.remove = imx_rt5640_remove,
};
module_platform_driver(imx_rt5640_driver);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("Freescale i.MX rt5640 ASoC machine driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:imx-rt5640");
