// File: slobodniklasteri.cpp

#include "slobodniklasteri.h"

#include "cluster.h"

void SlobodniKlasteri::dodaj(Partition* particija, ClusterNo klaster) {
  // Umetanje na pocetak liste slobodnih klastra

  Cluster head(particija, 0);
  // Sledeci je prvi u listi slobodnih klastra
  Cluster sledeci(particija, head._->free);
  Cluster novi(particija, klaster);

  // Trenutni klaster pokazuje na sledeci
  novi._->next = sledeci.no;

  //novi._->prev = head.no;

  //if(sledeci.no != 0)
  //  sledeci._->prev = novi.no;

  // Trenutni klaster postaje prvi u listi slobodnih klastera
  head._->free = novi.no;
}

ClusterNo SlobodniKlasteri::uzmi(Partition* particija) {
  // Uzimanje prvog slobodnog sa pocetka liste

  Cluster head(particija, 0);

  // Ako je lista prazna
  if(head._->free == 0)
    return 0;

  // Sledeci je prvi u listi slobodnih klastra
  Cluster sledeci(particija, head._->free);
  head._->free = sledeci._->next;

  /*if(sledeci._->next != 0) {
    Cluster sledeciSledeci(particija, sledeci._->next);
    sledeciSledeci._->prev = head.no;
  }*/

  //sledeci._->next = sledeci._->prev = 0;

  sledeci.clear();

  return sledeci.no;
}
