#!/usr/bin/env python
#coding=utf-8

#
# 项目派生相关配置替换脚本
# 2016-9-20 by zhong
#

import sys
import traceback
import os
import re
from optparse import OptionParser
from sys import stdout
import json
import codecs

def replace_base(res, config_json):
	print "*** 基础信息替换 ***"
	#ip匹配规则
	r_i = config_json["login_ip"]
	reg = r'yl.LOGONSERVER\s+= "(.+?)" --@login_ip'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[ip] " + cps + " ==> " + r_i
		res = re.sub(cps, r_i, res, 1)

	#登陆端口匹配规则
	r_p = str(config_json["login_port"])
	reg = r'yl.LOGONPORT\s+= (.+?) --@login_port'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[login_port] " + cps + " ==> " + r_p
		res = re.sub(cps, r_p, res, 1)

	#好友端口匹配规则
	r_fp = str(config_json["friend_port"])
	reg = r'yl.FRIENDPORT\s+= (.+?) --@friend_port'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[friend_port] " + cps + " ==> " + r_fp
		res = re.sub(cps, r_fp, res, 1)

	#URL匹配规则
	r_u = config_json["http_url"]
	reg = r'yl.HTTP_URL\s+= "(.+?)" --@http_url'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[url] " + cps + " ==> " + r_u
		res = re.sub(cps, r_u, res, 1)
	
	#编译码匹配
	r_c = config_json["compilation"]
	reg = r'yl.VALIDATE\s+= "(.+?)" --@compilation'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[compilation] " + cps + " ==> " + r_c
		res = re.sub(cps, r_c, res, 1)
	
	return res

def replace_share(res, config_json):
	print "*** 替换分享配置 ***"
	#分享标题 匹配规则
	r_t = config_json["share_title"]
	reg = r'title\s+= "(.+?)", --@share_title'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[title] " + cps + " ==> " + r_t
		res = re.sub(cps, r_t, res, 1)
	
	#分享内容 匹配规则
	r_c = config_json["share_content"]
	reg = r'content\s+= "(.+?)", --@share_content'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[content] " + cps + " ==> " + r_c
		res = re.sub(cps, r_c, res, 1)
	
	#友盟appkey 匹配规则
	r_a = config_json["share_appkey"]
	reg = r'AppKey\s+= "(.+?)", --@share_appkey'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[appkey] " + cps + " ==> " + r_a
		res = re.sub(cps, r_a, res, 1)
	
	return res

def replace_wechat(res, config_json):
	print "*** 替换微信配置 ***"
	#微信appid 匹配规则
	r_a = config_json["wechat_appid"]
	reg = r'AppID\s+= "(.+?)", --@wechat_appid'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[AppID] " + cps + " ==> " + r_a
		res = re.sub(cps, r_a, res, 1)
	
	#微信secret 匹配规则
	r_s = config_json["wechat_secret"]
	reg = r'AppSecret\s+= "(.+?)", --@wechat_secret'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[AppSecret] " + cps + " ==> " + r_s
		res = re.sub(cps, r_s, res, 1)
	
	#微信商户id 匹配规则
	r_p = config_json["wechat_partnerid"]
	reg = r'PartnerID\s+= "(.+?)", --@wechat_partnerid'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[PartnerID] " + cps + " ==> " + r_p
		res = re.sub(cps, r_p, res, 1)
	
	#微信支付密钥 匹配规则
	r_p = config_json["wechat_paykey"]
	reg = r'PayKey\s+= "(.+?)", --@wechat_paykey'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[PayKey] " + cps + " ==> " + r_p
		res = re.sub(cps, r_p, res, 1)
	
	return res

def replace_alipay(res, config_json):
	print "*** 替换支付宝配置 ***"
	#支付宝商户id 匹配规则
	r_a = config_json["alipay_partnerid"]
	reg = r'PartnerID\s+= "(.+?)", --@alipay_partnerid'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[PartnerID] " + cps + " ==> " + r_a
		res = re.sub(cps, r_a, res, 1)
	
	#支付宝收款id 匹配规则
	r_s = config_json["alipay_sellerid"]
	reg = r'SellerID\s+= "(.+?)", --@alipay_sellerid'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[SellerID] " + cps + " ==> " + r_s
		res = re.sub(cps, r_s, res, 1)
	
	#支付宝rsa密钥 匹配规则
	r_p = config_json["alipay_rsa"]
	reg = r'RsaKey\s+= "(.+?)", --@alipay_rsa'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[RsaKey] " + cps + " ==> " + r_p
		#res = re.sub(cps, r_p, res, 1)
		res = res.replace(cps, r_p)
	
	return res

def replace_jft(res, config_json):
	print "*** 替换竣付通配置 ***"
	#竣付通支付密钥 匹配规则
	r_a = config_json["jft_paykey"]
	reg = r'PayKey\s+= "(.+?)", --@jft_paykey'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[PayKey] " + cps + " ==> " + r_a
		res = re.sub(cps, r_a, res, 1)
	
	#竣付通商户id 匹配规则
	r_s = config_json["jft_partnerid"]
	reg = r'PartnerID\s+= "(.+?)", --@jft_partnerid'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[PartnerID] " + cps + " ==> " + r_s
		res = re.sub(cps, r_s, res, 1)
	
	#appid 匹配规则
	r_p = config_json["jft_appid"]
	reg = r'JftAppID\s+= "(.+?)", --@jft_appid'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[JftAppID] " + cps + " ==> " + r_p
		res = res.replace(cps, r_p)
	
	#aeskey 匹配规则
	r_p = config_json["jft_aeskey"]
	reg = r'JftAesKey\s+= "(.+?)", --@jft_aeskey'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[JftAesKey] " + cps + " ==> " + r_p
		res = re.sub(cps, r_p, res, 1)
	
	#aesvec 匹配规则
	r_p = config_json["jft_aesvec"]
	reg = r'JftAesVec\s+= "(.+?)", --@jft_aesvec'
	cpListS = re.findall(re.compile(reg),res)
	for cps in cpListS:
		print "[JftAesVec] " + cps + " ==> " + r_p
		res = re.sub(cps, r_p, res, 1)
	return res

def replace_yl(options, config_json):
	yl_file = options.proj_path + "/client/client/src/plaza/models/yl.lua"
	if True == os.path.exists(yl_file):
		print "\n替换 yl.lua"
		yf = codecs.open(yl_file, "r", "utf-8")
		res = yf.read()
		yf.close()
		
		#基础信息
		res = replace_base(res, config_json) 
		
		#分享配置
		res = replace_share(res, config_json)
		
		#微信配置
		res = replace_wechat(res, config_json)
		
		#支付宝配置
		res = replace_alipay(res, config_json)
		
		#竣付通配置
		res = replace_jft(res, config_json)
		
		yf = codecs.open(yl_file, "wb", 'utf-8')
		yf.write(res)
		yf.close()
	else:
		print yl_file + " missing"

def replace_welcome(options, config_json):
	we_file = options.proj_path + "/client/base/src/app/views/WelcomeScene.lua"
	if True == os.path.exists(we_file):
		print "\n替换 WelcomeScene.lua"
		yf = codecs.open(we_file, "r", "utf-8")
		res = yf.read()
		yf.close()
		
		r_u = config_json["http_url"]
		reg = r'local URL_REQUEST\s+= "(.+?)" --@http_url'
		cpListS = re.findall(re.compile(reg),res)
		for cps in cpListS:
			print "[url] " + cps + " ==> " + r_u
			res = re.sub(cps, r_u, res, 1)
		
		yf = codecs.open(we_file, "wb", 'utf-8')
		yf.write(res)
		yf.close()
	else:
		print we_file + " missing"

def replace_appdf(options, config_json):
	appdf_file = options.proj_path + "/client/base/src/app/models/AppDF.lua"
	if True == os.path.exists(appdf_file):
		print "\n替换 AppDF.lua"
		yf = codecs.open(appdf_file, "r", "utf-8")
		res = yf.read()
		yf.close()
		
		#程序版本归0
		r_u = str(config_json["app_version"])
		reg = r'BASE_C_VERSION\s+= (.+?) --@app_version'
		cpListS = re.findall(re.compile(reg),res)
		for cps in cpListS:
			print "[BASE_C_VERSION] " + cps + " ==> " + r_u
			res = re.sub(cps, r_u, res, 1)
		
		#大厅资源版本归0
		r_u = str(config_json["client_version"])
		reg = r'BASE_C_RESVERSION\s+= (.+?) --@client_version'
		cpListS = re.findall(re.compile(reg),res)
		for cps in cpListS:
			print "[BASE_C_RESVERSION] " + cps + " ==> " + r_u
			res = re.sub(cps, r_u, res, 1)
		
		yf = codecs.open(appdf_file, "wb", 'utf-8')
		yf.write(res)
		yf.close()
	else:
		print appdf_file + " missing"
def working(options):
	#读取配置文件
	print "******"
	print "导入配置文件"
	config_file = options.config_path + "/project.json"
	if False == os.path.exists(config_file):
		print("project.json missing")
		return
	f = codecs.open(config_file, "r")
	config_json = json.load(f)
	f.close()
	
	#替换 yl.lua
	replace_yl(options, config_json)
	
	#替换 WelcomeScene.lua
	replace_welcome(options, config_json)
	
	#替换AppDF.lua资源、大厅版本
	replace_appdf(options, config_json)
	
	#替换编译码
	define_file = options.proj_path + "/frameworks/runtime-src/Classes/GlobalDefine/Define.h"
	if True == os.path.exists(define_file):
		print "\n替换 Define.h"
		yf = codecs.open(define_file, "r", "utf-8")
		res = yf.read()
		yf.close()
		
		r_u = config_json["compilation"]
		reg = r'#define  szCompilation  "(.+?)" //@compilation'
		cpListS = re.findall(re.compile(reg),res)
		for cps in cpListS:
			print "[compilation] " + cps + " ==> " + r_u
			#全部替换
			res = re.sub(cps, r_u, res)
		
		yf = codecs.open(define_file, "wb", 'utf-8')
		yf.write(res)
		yf.close()
	else:
		print define_file + " missing"
	
	#替换游戏名
	android_file = options.proj_path + "/frameworks/runtime-src/proj.android/res/values/strings.xml"
	if True == os.path.exists(android_file):
		print "\n替换安卓版游戏名"
		yf = codecs.open(android_file, "r", "utf-8")
		res = yf.read()
		yf.close()
		
		r_u = config_json["game_title"]
		reg = r'name="app_name">(.+?)</string>'
		cpListS = re.findall(re.compile(reg),res)
		for cps in cpListS:
			print "[game_title] " + cps + " ==> " + r_u
			res = re.sub(cps, r_u, res, 1)
		
		yf = codecs.open(android_file, "wb", 'utf-8')
		yf.write(res)
		yf.close()
	else:
		print android_file + " missing"
	
	#替换游戏名
	ios_file = options.proj_path + "/frameworks/runtime-src/proj.ios_mac/ios/Info.plist"
	if True == os.path.exists(ios_file):
		print "\n替换苹果版游戏名"
		yf = codecs.open(ios_file, "r", "utf-8")
		res = yf.read()
		yf.close()
		
		r_u = config_json["game_title"]
		reg = r'<key>CFBundleDisplayName</key>\s+<string>(.+?)</string>\s+<key>CFBundleExecutable</key>'
		cpListS = re.findall(re.compile(reg),res)
		for cps in cpListS:
			print "[game_title] " + cps + " ==> " + r_u
			res = res.replace(cps, r_u, 1)
		
		yf = codecs.open(ios_file, "wb", 'utf-8')
		yf.write(res)
		yf.close()
	else:
		print ios_file + " missing"
	
def main():
	if(sys.version_info[0] > 2):
		print("Found Python interpreter : %s\n"%sys.version)
		print("This script works only with Python version up to 2.x *but* not with an above version")
		sys.exit(1)
	else:
		parser = OptionParser(usage="%prog -i <IFNAME> -o <opname>", version="%prog 0.3")
		parser.add_option("--projpath", "--projpath", dest="proj_path", help="project path")
		parser.add_option("--configpath", "--configpath", dest="config_path", help="config path")
		(options, args) = parser.parse_args()
		options.config_path = args[0]
		options.proj_path = args[1]
		
		#开始替换
		working(options)


# -------------- main --------------
if __name__ == '__main__':
	try:
		main()
	except Exception as e:
		traceback.print_exc()
		sys.exit(1)