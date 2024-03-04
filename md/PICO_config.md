# Configuration environnement PICO

# Requirements
Être sur du Linux parce que sur du Windows c'est la merde... désolé Lucas
```shell
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential 
```
## Git clone le SDK du pico

```shell
git clone https://github.com/raspberrypi/pico-sdk 
```

## Dans le projet ST2024Theremin

Dans le dossier :
```path
ST2024Theremin/bin/pico/c_lang
```

```shell
cd c_lang
mkdir build
cd build
export PICO_SDK_PATH=../../pico-sdk
```

## Créer l'exe pour le pico

```shell
cmake ..
make <nom_de_lexe>
```

Et mettre le fichier <nom_de_l'exe>.uf2 dans le pico

# La doc :

[Lien vers la doc](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
