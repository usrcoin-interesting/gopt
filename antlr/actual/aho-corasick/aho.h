#include<stdint.h>
#include "ds_queue.h"

#define AHO_SHM_KEY 1

#define AHO_MAX_STATES 32768	/**< Per-DFA max states */
#define AHO_FAIL 65535
#define AHO_ALPHA_SIZE 256

/**< Just some reasonable numbers */
#define AHO_MAX_PATTERNS (32 * 1024)
#define AHO_MAX_PATTERN_LEN (1024)
#define AHO_MAX_THREADS 16

/**< DFAs created with the following configuration:
  *  snort 2.9.7, conf: config detection: search-method ac-q */
#define AHO_MAX_DFA 450
#define AHO_PATTERN_FILE "../../../data_dump/snort/snort_dfa_patterns"

#define AHO_PACKET_FILE "../../../data_dump/snort/snort_packets"
#define AHO_MAX_PKTS (32 * 1024)	/**< Reading 2M packets takes a long time */
#define AHO_MAX_PKT_LEN (1460)

struct aho_dfa {
	int id;
	int num_used_states;
	struct aho_state *root;
};

struct aho_state {
	uint16_t G[AHO_ALPHA_SIZE];		/**< Goto function */
	uint16_t F;						/**< Failure function */
	struct ds_queue output;			/**< Output patterns at this state */
	uint8_t pad[32];
};

struct aho_pattern {
	int dfa_id;				/**< DFA to insert this pattern into */
	int len;				/**< Length of this pattern */
	uint8_t *content;		/**< Contents of this pattern */
};

struct aho_ctrl_blk {
	int tid;						/**< Thread ID */
	struct aho_dfa *dfa_arr;		/**< The shared DFAs */

	int num_pkts;					/**< Number of packets */
	struct aho_pkt *pkts;			/**< Packets to match in the DFAs */
};

struct aho_pkt {
	int pkt_id;				/**< An integer identifier for this packet */
	int dfa_id;				/**< DFA to use for matching this packet */
	int len;				/**< Length of this packet */
	uint8_t *content;
};

void aho_init(struct aho_dfa *dfa, int id);

void aho_add_pattern(struct aho_dfa *dfa, struct aho_pattern *pattern, int index);

void aho_build_ff(struct aho_dfa *dfa);
void aho_preprocess_dfa(struct aho_dfa *dfa);

struct aho_pattern* aho_get_strings(const char *filename, int *num_patterns);
struct aho_pattern* aho_get_patterns(const char *pat_file, int *num_patterns);

struct aho_pkt *aho_get_pkts(const char *pkt_file, int *num_pkts);
