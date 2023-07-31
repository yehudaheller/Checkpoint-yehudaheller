#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/inet.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MOSHE & YEHUDA");
MODULE_VERSION("0.01");

#define IP_ADDRESS "8.8.8.8" // for example google ip address

static unsigned int packet_logger_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *ip_header = ip_hdr(skb);

	if (ip_header->saddr == in_aton(IP_ADDRESS) || ip_header->daddr == in_aton(IP_ADDRESS))
	{
		printk(KERN_INFO "Dropped ip source: %pI4 destination: %pI4\n", &ip_header->saddr, &ip_header->daddr);
		return NF_DROP;
	}
	return NF_ACCEPT;
}

static struct nf_hook_ops nfho = {
	.hook = packet_logger_hook,
	.pf = PF_INET,
	.hooknum = NF_INET_PRE_ROUTING,
	.priority = NF_IP_PRI_FIRST,
};

static int __init LKM_init(void)
{
	if (nf_register_net_hook(&init_net, &nfho))
	{
		printk(KERN_ERR "Failed to register netfilter hook.\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "Packet logger module loaded.\n");
	return 0;
}

static void __exit LKM_exit(void)
{
	nf_unregister_net_hook(&init_net, &nfho);
	printk(KERN_INFO "Packet logger module unloaded.\n");
}

module_init(LKM_init);
module_exit(LKM_exit);
