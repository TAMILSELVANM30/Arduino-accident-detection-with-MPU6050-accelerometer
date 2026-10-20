"""Accident dashboard: reads the Nano, asks Y/N, sends email with location."""
import smtplib, sys, time
from datetime import datetime
from email.mime.text import MIMEText
import serial
import config

try:
    import msvcrt            # Windows: non-blocking keyboard
    import winsound
except ImportError:
    msvcrt = None
    winsound = None

def beep():
    if winsound:
        for _ in range(3):
            winsound.Beep(1200, 300)

def ask_yes_no(timeout):
    """Return 'Y', 'N' or None (timeout)."""
    print(f"Real accident? Type Y or N within {timeout}s: ", end="", flush=True)
    end = time.time() + timeout
    if msvcrt:
        while time.time() < end:
            if msvcrt.kbhit():
                ch = msvcrt.getwch().upper()
                if ch in ("Y", "N"):
                    print(ch)
                    return ch
            time.sleep(0.05)
        print()
        return None
    return input().strip().upper()[:1] or None   # fallback (no timeout) for Linux/Mac

def send_email(x, y, z):
    body = (f"ACCIDENT ALERT!\n\nTime: {datetime.now():%Y-%m-%d %H:%M:%S}\n\n"
            f"Acceleration: X={x} Y={y} Z={z}\n\n"
            f"Location: {config.LATITUDE}, {config.LONGITUDE}\n"
            f"Map: https://maps.google.com/?q={config.LATITUDE},{config.LONGITUDE}\n")
    msg = MIMEText(body)
    msg["Subject"] = "ACCIDENT DETECTED"
    msg["From"] = config.SENDER_EMAIL
    msg["To"] = config.RECEIVER_EMAIL
    try:
        with smtplib.SMTP(config.SMTP_HOST, config.SMTP_PORT) as s:
            s.starttls()
            s.login(config.SENDER_EMAIL, config.SENDER_PASSWORD)
            s.send_message(msg)
        print("Email sent to", config.RECEIVER_EMAIL)
    except Exception as e:
        print("Email failed:", e)

def main():
    try:
        ser = serial.Serial(config.PORT, config.BAUD, timeout=1)
    except Exception as e:
        sys.exit(f"Cannot open {config.PORT}: {e}\nClose Arduino Serial Monitor and check the port.")
    time.sleep(2)
    print("Connected. Waiting for data... (Ctrl+C to quit)")
    x = y = z = 0
    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if line.startswith("X:"):
                try:
                    p = line.split()
                    x, y, z = (int(v.split(":")[1]) for v in p[:3])
                    print(f"X:{x:>7} Y:{y:>7} Z:{z:>7}")
                except ValueError:
                    pass
            elif "ACCIDENT_DETECTED" in line:
                print("\n" + "=" * 50 + "\nACCIDENT DETECTED!\n" + "=" * 50)
                print(f"X:{x} Y:{y} Z:{z}\nLocation: {config.LATITUDE}, {config.LONGITUDE}")
                beep()
                ans = ask_yes_no(config.CONFIRM_TIMEOUT)
                if ans == "N":
                    ser.write(b"N")
                    print("False alert - alarm stopped.\n")
                else:
                    ser.write(b"Y")
                    print("Confirmed" if ans == "Y" else "No response - treating as REAL accident")
                    send_email(x, y, z)
        except KeyboardInterrupt:
            print("\nBye"); break

if __name__ == "__main__":
    main()
