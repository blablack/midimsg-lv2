#!/usr/bin/env python
import os
import subprocess
import shutil
from waflib import Logs
from waflib.extras import autowaf as autowaf

# Variables for 'waf dist'
APPNAME = 'midimsg-lv2'
VERSION = '0.0.2'

# Mandatory variables
top = '.'
out = 'build'


def options(opt):
    autowaf.set_options(opt)
    opt.load('compiler_cxx')
    
    
def configure(conf):
    autowaf.configure(conf)
    autowaf.display_header('midimsg-lv2 Configuration')
    
    conf.load('compiler_cxx')
    
    autowaf.check_pkg(conf, 'lv2', uselib_store='LV2', atleast_version='1.2.0')
        
    # Set env['pluginlib_PATTERN']
    pat = conf.env['cxxshlib_PATTERN']
    if pat[0:3] == 'lib':
        pat = pat[3:]
    conf.env['pluginlib_PATTERN'] = pat

    autowaf.display_msg(conf, "LV2 bundle directory", conf.env['LV2DIR'])
    print('')


def build_plugin(bld, bundle, name, source, cxxflags=[], libs=[], add_source=[]):
    penv = bld.env.derive()
    penv['cxxshlib_PATTERN'] = bld.env['pluginlib_PATTERN']
    obj              = bld(features = 'cxx cxxshlib')
    obj.env          = penv
    obj.source       = source + add_source
    obj.includes     = ['.', './src']
    obj.name         = name
    obj.target       = os.path.join(bundle, name)
    if cxxflags != []:
        obj.cxxflags = cxxflags
    if libs != []:
	obj.uselib = libs
    obj.install_path = '${LV2DIR}/' + bundle

    # Install data file
    data_file = '%s.ttl' % name
    bld.install_files('${LV2DIR}/' + bundle, os.path.join(bundle, data_file))

def build(bld):
	def do_copy(task):
		src = task.inputs[0].abspath()
		tgt = task.outputs[0].abspath()
		return shutil.copy(src, tgt)
    
	for i in bld.path.ant_glob('midimsg.lv2/*.ttl'):
		bld(rule   = do_copy,
            source = i,
            target = bld.path.get_bld().make_node('midimsg.lv2/%s' % i),
            install_path = '${LV2DIR}/midimsg.lv2')

 	plugins = '''
	controller
	modwheel
	aftertouch
	'''.split()

	for i in plugins:
		build_plugin(bld, 'midimsg.lv2', i, ['src/%s.cpp' % i],
			['-DPLUGIN_CLASS=%s' % i,
			'-std=c++11',
            '-DURI_PREFIX=\"http://github.com/blablack/midimsg-lv2/\"',
            '-DPLUGIN_URI_SUFFIX="%s"' % i,
            '-DPLUGIN_HEADER="src/%s.hpp"' % i],
		  	['LV2', 'LVTK_PLUGIN'],
		  	[])
		  	
	
