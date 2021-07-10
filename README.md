# rmusbbdev

## Motivação

Programinha para uso em scripts de backup, com propósito de _remover com segurança_ dispositivos de armazenamento USB **após desmontados**. O kernel liberará o cache de escrita caso exista e desativará a porta. Equivale a `udisksctl power-off …`, porém depende apenas da libudev, sem instalar a montanha de coisa requerida pelo udisks.

## Gerando binário

#### Pacotes requeridos

Fedora/CentOS:

* `gcc`
* `systemd-devel`

Debian/Ubuntu:

* `gcc`
* `libc-dev`
* `libudev-dev`

#### Compilação

```
gcc rmusbbdev.c -o rmusbbdev -ludev
```

## Uso

```
rmusbbdev /dev/dispositivo
```

Pode ser especificado o dispositivo principal (`/dev/sdx`) ou partição (`/dev/sdxy`).
