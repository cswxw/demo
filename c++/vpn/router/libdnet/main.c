
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
	/* ��ʾ·�ɱ��¼ */
	return (0);
}
int main(int argc, char *argv[])
{
	struct route_entry entry;
	/* ·�ɱ�����ݽṹ�ı��� */
	route_t *handle;
	/* ·�ɱ������� */
	handle = route_open();
	/* ��·�ɱ���� */
	printf("Show the route rules:\n");
	printf("Route rules :\n");
	printf("Destination IP  ----------------------- Gateway IP\n");
	route_loop(handle, show_route_rule, NULL);
	/*
	* ע��ص�����show_route_rule��ѭ�����ûص�������·�ɱ���в�������������ʾ����·�ɱ���Ϣ
	*/
	printf("Show the route rules about the Destination IP:\n");
	addr_aton("192.168.0.4", &entry.route_dst);
	/* ���ַ�����ʽ��IP��ַת��Ϊ��������ʽ��IP��ַ */
	route_get(handle, &entry);
	/* ���ݵ�ַ��ȡ��·�ɱ���Ϣ */
	printf("Destination is :\n");
	printf("%s\n", addr_ntoa(&entry.route_dst));
	/* ���Ŀ�ĵ�ַ */
	printf("Gateway is: \n");
	printf("%s\n", addr_ntoa(&entry.route_gw));
	/* ������ص�ַ */
	printf("Add a route rule :\n");
	addr_aton("192.168.0.5", &entry.route_dst);
	/* ���ַ�����ʽ��IP��ַת��Ϊ��������ʽ��IP��ַ */
	addr_aton("192.168.0.10", &entry.route_gw);
	/* ���ַ�����ʽ��IP��ַת��Ϊ��������ʽ��IP��ַ */
	route_add(handle, &entry);
	/* ���·�ɱ��¼ */
	printf("Destination :%s Gateway :%s added:\n", addr_ntoa(&entry.route_dst), addr_ntoa(&entry.route_gw));
	printf("Add a route rule :\n");
	addr_aton("192.168.0.6", &entry.route_dst);
	/* ���ַ�����ʽ��IP��ַת��Ϊ��������ʽ��IP��ַ */
	addr_aton("192.168.0.20", &entry.route_gw);
	/* ���ַ�����ʽ��IP��ַת��Ϊ��������ʽ��IP��ַ */
	route_add(handle, &entry);
	/* ���·�ɱ��¼ */
	printf("Destination :%s Gateway :%s added:\n", addr_ntoa(&entry.route_dst), addr_ntoa(&entry.route_gw));
	printf("Show the route rules:\n");
	printf("Route rules :\n");
	printf("Destination IP  ----------------------- Gateway IP\n");
	route_loop(handle, show_route_rule, NULL);
	/* ��ʾ����·�ɱ��¼ */
	printf("Delete a route rule :\n");
	addr_aton("192.168.0.5", &entry.route_dst);
	/* ���ַ�����ʽ��IP��ַת��Ϊ��������ʽ��IP��ַ */
	route_delete(handle, &entry);
	/* ɾ��·�ɱ��¼ */
	printf("Destination :%s deleted\n", addr_ntoa(&entry.route_dst));
	printf("Show the route rules:\n");
	printf("Route rules :\n");
	printf("Destination IP  ----------------------- Gateway IP\n");
	route_loop(handle, show_route_rule, NULL);
	/* ����ʾ����·�ɱ��¼ */
	route_close(handle);
	/* �ر�·�ɱ���� */
}