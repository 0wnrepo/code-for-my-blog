#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main()
{
	const char *src_file = "./original.pcap";
	const char *dst_file = "./copy.pcap";
	pcap_t *src_handle = NULL;
	pcap_t *dst_handle = NULL;
	int number = 1200;

	char err[PCAP_ERRBUF_SIZE];
	src_handle = pcap_open_offline(src_file, err);
	if (src_handle == NULL) {
		printf("Open %s failed: %s\n", src_file, err);
		return EXIT_FAILURE;
	}

	dst_handle = pcap_open_dead(DLT_EN10MB, 262144);
	pcap_dumper_t *dst_dump = pcap_dump_open(dst_handle, dst_file);
	if (dst_dump == NULL) {
		printf("pcap_dump_open error: %s\n", pcap_geterr(dst_handle));
		return EXIT_FAILURE;
	}

	while (1)
	{
		struct pcap_pkthdr *hdr = NULL;
		const u_char *data = NULL;
		static int count = 0;

		int ret = pcap_next_ex(src_handle, &hdr, &data);
		if (ret == 1) {
			if (count++ == number) {
				break;
			} else {
				pcap_dump((u_char *)dst_dump, hdr, data);
			}
		} else if (ret == PCAP_ERROR_BREAK) {
			break;
		} else {
			printf("pcap_next_ex error: %s\n", pcap_geterr(src_handle));
			return EXIT_FAILURE;
		}
	}

	pcap_dump_close(dst_dump);
	pcap_close(dst_handle);
	pcap_close(src_handle);

	return EXIT_SUCCESS;
}
