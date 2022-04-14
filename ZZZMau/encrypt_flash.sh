port=COM3

echo erase full flash
idf.py -p $port erase_flash

echo generate encrypt flash key
espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

echo burn key to flash
espefuse.py -p $port burn_key flash_encryption my_flash_encryption_key.bin

espefuse.py -p $port burn_efuse FLASH_CRYPT_CONFIG 0xf

espefuse.py -p $port burn_efuse FLASH_CRYPT_CNT

idf.py encrypted-flash monitor
