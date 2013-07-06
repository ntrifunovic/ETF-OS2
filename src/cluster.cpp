#include "cluster.h"

map<pair<Partition*, ClusterNo>, pair<_Cluster*, int> > Cluster::kes;
map<pair<Partition*, ClusterNo>, _Cluster*> Cluster::lru;
queue<pair<Partition*, ClusterNo>> Cluster::lru_queue;

void Cluster::load() {
  if(kes.find(make_pair(particija, no)) == kes.end()) {
	  if(lru.find(make_pair(particija, no)) == lru.end()) {
		lru[make_pair(particija, no)] =  new _Cluster();
		lru_queue.push(make_pair(particija, no));

		particija->readCluster(no, (char *)lru[make_pair(particija, no)]);

		if(lru.size() > LRU_SIZE) {
			if(kes.find(lru_queue.front()) == kes.end()) {
				lru_queue.front().first->writeCluster(lru_queue.front().second, (char *)lru[lru_queue.front()]);
				delete lru[lru_queue.front()];
			}

			lru.erase(lru_queue.front());
			lru_queue.pop();
		}
	  }

    kes[make_pair(particija, no)].first =  lru[make_pair(particija, no)];  
  }

  kes[make_pair(particija, no)].second++;
  _ = kes[make_pair(particija, no)].first;
}

void Cluster::save() {
  kes[make_pair(particija, no)].second--;

  if(kes[make_pair(particija, no)].second == 0) {
	  if(lru.find(make_pair(particija, no)) == lru.end()) {
		  particija->writeCluster(no, (char *)_);
		  delete kes[make_pair(particija, no)].first;
	  }
	
	  kes.erase(make_pair(particija, no));
  }
}