// vim: ts=2 sw=2 noexpandtab

#ifndef UART_DECODER_H
#define UART_DECODER_H

/*
 * Decode 1 byte received over UART.
 *
 * \param data   The received byte.
 * \returns      A pointer to the decoded frame buffer, when ready, NULL otherswise.
 */
uint8_t* uart_decode( uint8_t data );

#endif // UART_DECODER_H
