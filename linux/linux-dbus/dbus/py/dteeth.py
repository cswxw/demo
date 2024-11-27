#!/usr/bin/env python
# -*- coding: utf-8 -*-

import dbus
import _introspect_parser
import getopt, sys

MARGIN_WIDTH = 4
ONE_MARGIN = ' ' * MARGIN_WIDTH

# signal是个元组，它有一个元素，是一个列表。列表的元素是signal的参数
# 列表的每个元素都是字典。它有两个元素，键值分别是'type'和'name'
def show_signal(name, signal, margin):
    print margin+name+'(',
    args = signal[0]
    for i, arg in enumerate(args):
        if i > 0:
            print ',',
        if arg['name']:
            print '%s %s' % (arg['type'], arg['name']),
        else:
            print '%s' % arg['type'],
    print  ')'

# method是个元组，它有两个元素，都是列表。前一个列表的元素是输入参数，后一个列表的元素是输出参数
def show_method(name, method, margin):
    print margin+name+'(',
# 输入参数
    args = method[0]
    in_num = len(args)
    out_num = len(method[1])
    for i, arg in enumerate(args):
        if i > 0:
            print ',',
        if arg['name']:
            print 'in %s %s' % (arg['type'], arg['name']),
        else:
            print 'in %s' % arg['type'],
# 输出参数
    if (in_num > 0) and (out_num > 0) :
        print ',',
    args = method[1]
    for i, arg in enumerate(args):
        if i > 0:
            print ',',
        if arg['name']:
            print 'out %s %s' % (arg['type'], arg['name']),
        else:
            print 'out %s' % arg['type'],
    print  ')'

def show_property(name, property, margin):
    print margin+name
    print margin,
    print property

# interfaces是个字典，它有三个元素，键值分别是'signals'、'methods'和'properties'
def show_iface(name, iface, margin):
    print margin + name
    margin += ONE_MARGIN
    signals=iface['signals']
    l = len(signals)
    if l > 0:
        print margin+'signals'
        for node in signals:
            show_signal(node, signals[node], margin+ONE_MARGIN)

    methods=iface['methods']
    l = len(methods)
    if l > 0:
        print margin+'methods'
        for node in methods:
            show_method(node, methods[node], margin+ONE_MARGIN)

    properties=iface['properties']
    l = len(properties)
    if l > 0:
        print margin+'properties'
        for node in properties:
            show_property(node, properties[node], margin+ONE_MARGIN)

def show_obj(bus, name, obj_name, margin):
    obj=bus.get_object(name, obj_name)
    iface=dbus.Interface(obj, 'org.freedesktop.DBus.Introspectable')
    xml=iface.Introspect();
    data = _introspect_parser.process_introspection_data(xml)

    # data是个字典，它有两个元素，键值分别是'child_nodes'和'interfaces'
    if len(data['interfaces']) > 0:
    	print margin + obj_name

    for node in data['interfaces']:
        iface=data['interfaces'][node]
        show_iface(node, iface, margin+ONE_MARGIN)

    for node in data['child_nodes']:
        if obj_name == '/':
            show_obj(bus, name, '/' + node, margin)
        else:
            show_obj(bus, name, obj_name + '/' + node, margin)

def show_connection(bus, name, margin):
    print margin + name
    show_obj(bus, name, '/', margin+ONE_MARGIN)

def usage():
    print "Usage: dteeth [--system] <name of a connection on the bus >"
    
def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help", "system"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    if len(args) == 0:
        usage()
        sys.exit(2)

    use_system = False
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        if o == "--system":
            use_system = True
        else:
            assert False, "unhandled option"

    if use_system:
        bus=dbus.SystemBus()
    else:
        bus=dbus.SessionBus()

    for arg in args:
        show_connection(bus, arg, "")

if __name__ == "__main__":
    main()
