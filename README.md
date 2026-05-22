RASPBERRY PI Pico 2 W
======================

Documentations de la carte [ICI](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)

Projet 1: 
--------------------

entrées/sorties numériques et analogiques
Vanne1=PB2 / Vanne2=PB4 / Tension(entrée)=PB26 / Tension(sortie)=PB5
Une page HTML/CSS/JS


```
rm -r build/
```
```
cmake -DPICO_BOARD=pico2_w -B build
```
```
cmake --build build -j$(nproc)
```
```
cp build/server.uf2 /media/jb/RP2350/
```

Les dossiers:

Les entrées/sortie de base
/gpio_es/
HTTPS/TLS de base (version debug)
/https_tls/
HTTPS/TLS de base (version terminée)
/https_tls_simplifié/https_tls/
HTTPS/TLS de base (version simplifiée)
/https_tls_ultrasimplifié/https_tls/

Le développement:
```
openssl req -x509 -newkey rsa:2048 \
-keyout server.key \
-out server.crt \
-days 365 \
-nodes \
-sha256 \
-subj "/CN=pico.local"

awk '{print "\"" $0 "\\n\""}' server.crt
awk '{print "\"" $0 "\\n\""}' server.key
```


les débugs se font:
/root/pico/pico-sdk/src/rp2_common/pico_lwip/altcp_tls_mbedtls.c

les tests se font:
```
curl -vk https://192.168.1.208/
```