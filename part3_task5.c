#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MOSHE & YEHUDA");
MODULE_VERSION("0.01");

static struct nf_hook_ops *nfho;

static unsigned int packet_logger_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *ip_header = ip_hdr(skb);
    int src_ip = (int)ip_header->saddr;
    int dest_ip = (int)ip_header->daddr;
    printk(KERN_INFO "src: %pI4 dest: %pI4\n", &src_ip, &dest_ip);
    return NF_ACCEPT;
}

static int register_hook(void)
{
    nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
    if (!nfho) {
        printk(KERN_ERR "Failed to allocate memory for nf_hook_ops\n");
        return -ENOMEM;
    }

    nfho->hook = packet_logger_hook;
    nfho->hooknum = NF_INET_PRE_ROUTING;
    nfho->pf = PF_INET;
    nfho->priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, nfho);
    return 0;
}

static void unregister_hook(void)
{
    if (nfho) {
        nf_unregister_net_hook(&init_net, nfho);
        kfree(nfho);
    }
}

static int __init LKM_init(void)
{
    int ret = register_hook();
    if (ret != 0)
        return ret;

    return 0;
}

static void __exit LKM_exit(void)
{
    unregister_hook();
}

module_init(LKM_init);
module_exit(LKM_exit);
