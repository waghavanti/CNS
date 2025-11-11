from PIL import Image
import numpy as np

def encrypt_image(input_path, shift):
    image = Image.open(input_path)
    pixels = np.array(image).astype(np.int16)

    encrypted_pixels = ((pixels + shift) % 256).astype(np.uint8)

    encrypted_image = Image.fromarray(encrypted_pixels)
    encrypted_image.save("encrypted_image.png")
    print(" Encrypted image saved as encrypted_image.png")


def decrypt_image(input_path, shift):
    image = Image.open(input_path)
    pixels = np.array(image).astype(np.int16)

    decrypted_pixels = ((pixels - shift) % 256).astype(np.uint8)

    decrypted_image = Image.fromarray(decrypted_pixels)
    decrypted_image.save("decrypted_image.png")
    print("Decrypted image saved as decrypted_image.png")



print("---- Caesar Cipher Image Encryption/Decryption ----")
print("1. Encrypt Image (Viewable)")
print("2. Decrypt Image (Viewable)")
choice = input("Enter your choice (1 or 2): ")

if choice == "1":
    input_path = input("Enter the path to the original image file: ")
    shift = int(input("Enter the shift value (1-255): "))
    encrypt_image(input_path, shift)

elif choice == "2":
    input_path = input("Enter the path to the encrypted image file: ")
    shift = int(input("Enter the shift value used for encryption (1-255): "))
    decrypt_image(input_path, shift)

else:
    print(" Invalid choice! Please enter 1 or 2.")