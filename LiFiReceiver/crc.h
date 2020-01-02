unsigned char calc_crc(char * msg, int msg_length)
{
    unsigned char crc = 0;   // Resultat sur 8 bits
    //--- Calcul de la somme, le premier caractère est en position 1
    for (int i = 0; i < msg_length; i++)
        crc += msg[i];
    //--- Modulo Valeur Maxi sur 8 Bits
    return (crc %= 0xFF);
}
