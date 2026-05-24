RASPBERRY PI Pico 2 W
======================

Documentations de la carte [ICI](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)

Projet 1: 
--------------------

entrées/sorties numériques et analogiques

Vanne1=PB2 / Vanne2=PB4 / Tension(entrée)=PB26 / Tension(sortie)=PB5

Une page HTML/CSS/JS séparée charger sur port 80

APIs sur port 443


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

Le dossier back (anciens codes dans lequel je me suis basé):
<table border="1">
<tr><td>/gpio_es/</td><td>Les entrées/sortie de base</td></tr>
<tr><td>/https_tls/</td><td>HTTPS/TLS de base (version debug)</td></tr>
<tr><td>/https_tls_simplifié/https_tls/</td><td>HTTPS/TLS de base (version terminée)</td></tr>
<tr><td>/https_tls_ultrasimplifié/https_tls/</td><td>HTTPS/TLS de base (version simplifiée)</td></tr>
</table>

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


les débugs https/tls se font:

/root/pico/pico-sdk/src/rp2_common/pico_lwip/altcp_tls_mbedtls.c

les tests se font:
```
curl -vk https://192.168.1.208/
```

le déploiement:
```
xxd -i index.html > index_html.h
xxd -i style.css > style_css.h
terser script.js -c -m -o script.min.js
xxd -i script.min.js > script_min_js.h
```
si changement IP, bien vérifier dans route.c en plus du wifi.c, car problèmes CORS qui doivent être réglé (confrontation entre un protocole sécurisé et non sécurisé)
