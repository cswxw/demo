
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dnet.h>
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"dnet.lib")

#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

int show_route_rule(const struct route_entry *entry, void *arg)
{
	printf("Destination :%s  =======  Gateway :%s\n", addr_ntoa(&entry->route_dst), addr_ntoa(&entry->route_gw));
	/* 显示路由表记录 */
	return (0);
}
int main(int argc, char *argv[])
{
	struct route_entry entry;
	/* 路由表的数据结构的变量 */
	route_t *handle;
	/* 路由表操作句柄 */
	handle = route_open();
	/* 打开路由表操作 */
	printf("Show the route rules:\n");
	printf("Route rules :\n");
	printf("Destination IP  ----------------------- Gateway IP\n");
	route_loop(handle, show_route_rule, NULL);
	/*
	* 注册回调函数show_route_rule，循环调用回调函数对路由表进行操作，这里是显示所有路由表信息
	*/
	printf("Show the route rules about the Destination IP:\n");
	addr_aton("192.168.0.4", &entry.route_dst);
	/* 把字符串形式的IP地址转换为二进制形式的IP地址 */
	route_get(handle, &entry);
	/* 根据地址获取其路由表信息 */
	printf("Destination is :\n");
	printf("%s\n", addr_ntoa(&entry.route_dst));
	/* 输出目的地址 */
	printf("Gateway is: \n");
	printf("%s\n", addr_ntoa(&entry.route_gw));
	/* 输出网关地址 */
	printf("Add a route rule :\n");
	addr_aton("192.168.0.5", &entry.route_dst);
	/* 把字符串形式的IP地址转换为二进制形式的IP地址 */
	addr_aton("192.168.0.10", &entry.route_gw);
	/* 把字符串形式的IP地址转换为二进制形式的IP地址 */
	route_add(handle, &entry);
	/* 添加路由表记录 */
	printf("Destination :%s Gateway :%s added:\n", addr_ntoa(&entry.route_dst), addr_ntoa(&entry.route_gw));
	printf("Add a route rule :\n");
	addr_aton("192.168.0.6", &entry.route_dst);
	/* 把字符串形式的IP地址转换为二进制形式的IP地址 */
	addr_aton("192.168.0.20", &entry.route_gw);
	/* 把字符串形式的IP地址转换为二进制形式的IP地址 */
	route_add(handle, &entry);
	/* 添加路由表记录 */
	printf("Destination :%s Gateway :%s added:\n", addr_ntoa(&entry.route_dst), addr_ntoa(&entry.route_gw));
	printf("Show the route rules:\n");
	printf("Route rules :\n");
	printf("Destination IP  ----------------------- Gateway IP\n");
	route_loop(handle, show_route_rule, NULL);
	/* 显示所有路由表记录 */
	printf("Delete a route rule :\n");
	addr_aton("192.168.0.5", &entry.route_dst);
	/* 把字符串形式的IP地址转换为二进制形式的IP地址 */
	route_delete(handle, &entry);
	/* 删除路由表记录 */
	printf("Destination :%s deleted\n", addr_ntoa(&entry.route_dst));
	printf("Show the route rules:\n");
	printf("Route rules :\n");
	printf("Destination IP  ----------------------- Gateway IP\n");
	route_loop(handle, show_route_rule, NULL);
	/* 再显示所有路由表记录 */
	route_close(handle);
	/* 关闭路由表操作 */
}