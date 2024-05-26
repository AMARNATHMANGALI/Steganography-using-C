Steganography in C
Project Description:
  This repository contains an implementation of steganography in C, a technique for hiding information within a digital medium. Steganography is often used to conceal a message within an image or audio file in such a way that it is undetectable to the casual observer. This project focuses on embedding and extracting messages within bitmap images (BMP files).

Features:
Image Encoding: Hide a secret message within a BMP image using the least significant bit (LSB) method.
Image Decoding: Extract a hidden message from a BMP image.
Command-Line Interface: User-friendly command-line interface to encode and decode messages.
Error Handling: Robust error handling for invalid inputs and corrupted images.
Cross-Platform: Compatible with multiple operating systems including Windows, macOS, and Linux.

Prerequisites
A C compiler (GCC recommended)
BMP image files for encoding and decoding

How It Works:
Encoding: The program reads the BMP image and the secret message. Each character of the message is converted to its binary representation and embedded into the least significant bits of the image pixels.
Decoding: The program reads the BMP image and extracts the least significant bits from the pixels to reconstruct the hidden message.

Acknowledgements:
Inspiration from various steganography tutorials and resources.
BMP image handling based on public domain BMP libraries.
