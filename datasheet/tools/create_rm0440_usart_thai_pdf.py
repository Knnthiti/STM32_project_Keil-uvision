from __future__ import annotations

from html import escape
from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    LongTable,
    PageBreak,
    Paragraph,
    SimpleDocTemplate,
    Spacer,
    Table,
    TableStyle,
)


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "output" / "pdf" / "RM0440_USART_UART_pages_1689_1774_TH.pdf"


def register_fonts() -> tuple[str, str]:
    regular = Path(r"C:\Windows\Fonts\tahoma.ttf")
    bold = Path(r"C:\Windows\Fonts\tahomabd.ttf")
    if not regular.exists() or not bold.exists():
        raise FileNotFoundError("Thai-capable Tahoma fonts were not found in C:\\Windows\\Fonts")
    pdfmetrics.registerFont(TTFont("Tahoma", str(regular)))
    pdfmetrics.registerFont(TTFont("Tahoma-Bold", str(bold)))
    return "Tahoma", "Tahoma-Bold"


FONT, FONT_BOLD = register_fonts()


def make_styles():
    base = getSampleStyleSheet()
    normal = ParagraphStyle(
        "ThaiNormal",
        parent=base["Normal"],
        fontName=FONT,
        fontSize=10.2,
        leading=15,
        textColor=colors.HexColor("#202124"),
        alignment=TA_LEFT,
        wordWrap="CJK",
        spaceAfter=4,
    )
    small = ParagraphStyle(
        "ThaiSmall",
        parent=normal,
        fontSize=8.3,
        leading=11.4,
        spaceAfter=2,
        wordWrap="CJK",
    )
    tiny = ParagraphStyle(
        "ThaiTiny",
        parent=normal,
        fontSize=7.1,
        leading=9.4,
        spaceAfter=1,
        wordWrap="CJK",
    )
    title = ParagraphStyle(
        "ThaiTitle",
        parent=normal,
        fontName=FONT_BOLD,
        fontSize=22,
        leading=30,
        alignment=TA_CENTER,
        textColor=colors.HexColor("#0F3D57"),
        spaceAfter=12,
    )
    subtitle = ParagraphStyle(
        "ThaiSubtitle",
        parent=normal,
        fontSize=12.5,
        leading=18,
        alignment=TA_CENTER,
        textColor=colors.HexColor("#3D4A54"),
        spaceAfter=12,
    )
    h1 = ParagraphStyle(
        "ThaiH1",
        parent=normal,
        fontName=FONT_BOLD,
        fontSize=16,
        leading=22,
        textColor=colors.HexColor("#123F5A"),
        spaceBefore=10,
        spaceAfter=7,
        keepWithNext=True,
    )
    h2 = ParagraphStyle(
        "ThaiH2",
        parent=normal,
        fontName=FONT_BOLD,
        fontSize=13,
        leading=18,
        textColor=colors.HexColor("#1B5C73"),
        spaceBefore=8,
        spaceAfter=5,
        keepWithNext=True,
    )
    h3 = ParagraphStyle(
        "ThaiH3",
        parent=normal,
        fontName=FONT_BOLD,
        fontSize=11.2,
        leading=15.5,
        textColor=colors.HexColor("#293B44"),
        spaceBefore=5,
        spaceAfter=3,
        keepWithNext=True,
    )
    note = ParagraphStyle(
        "ThaiNote",
        parent=normal,
        fontSize=9.2,
        leading=13.5,
        leftIndent=8,
        rightIndent=8,
        textColor=colors.HexColor("#38444D"),
        backColor=colors.HexColor("#EEF5F7"),
        borderColor=colors.HexColor("#B7D3DC"),
        borderWidth=0.6,
        borderPadding=6,
        spaceBefore=4,
        spaceAfter=7,
        wordWrap="CJK",
    )
    cover_note = ParagraphStyle(
        "ThaiCoverNote",
        parent=note,
        alignment=TA_CENTER,
        fontSize=10,
        leading=15,
        borderPadding=8,
    )
    return {
        "normal": normal,
        "small": small,
        "tiny": tiny,
        "title": title,
        "subtitle": subtitle,
        "h1": h1,
        "h2": h2,
        "h3": h3,
        "note": note,
        "cover_note": cover_note,
    }


STYLES = make_styles()


def p(text: str, style: str = "normal") -> Paragraph:
    return Paragraph(escape(text).replace("\n", "<br/>"), STYLES[style])


def rich(text: str, style: str = "normal") -> Paragraph:
    # Use only for strings that intentionally contain simple ReportLab markup.
    return Paragraph(text, STYLES[style])


def bullets(items: list[str], style: str = "normal") -> list:
    flow = []
    for item in items:
        flow.append(p("- " + item, style))
    return flow


def tbl(rows: list[list[str]], widths: list[float] | None = None, tiny: bool = False, repeat: int = 1):
    if widths:
        page_width = A4[0] - 2 * 17 * mm
        col_widths = [page_width * w for w in widths]
    else:
        col_widths = None
    cell_style = STYLES["tiny" if tiny else "small"]
    data = [[p(str(cell), "tiny" if tiny else "small") for cell in row] for row in rows]
    table = LongTable(data, colWidths=col_widths, repeatRows=repeat, hAlign="LEFT")
    table.setStyle(
        TableStyle(
            [
                ("FONT", (0, 0), (-1, -1), FONT),
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#DDECF1")),
                ("TEXTCOLOR", (0, 0), (-1, 0), colors.HexColor("#123F5A")),
                ("FONT", (0, 0), (-1, 0), FONT_BOLD),
                ("ALIGN", (0, 0), (-1, -1), "LEFT"),
                ("VALIGN", (0, 0), (-1, -1), "TOP"),
                ("GRID", (0, 0), (-1, -1), 0.35, colors.HexColor("#B8C9D0")),
                ("ROWBACKGROUNDS", (0, 1), (-1, -1), [colors.white, colors.HexColor("#F7FAFB")]),
                ("LEFTPADDING", (0, 0), (-1, -1), 4),
                ("RIGHTPADDING", (0, 0), (-1, -1), 4),
                ("TOPPADDING", (0, 0), (-1, -1), 3),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 3),
            ]
        )
    )
    return table


def section(story: list, heading: str, paras: list[str] | None = None):
    story.append(p(heading, "h1"))
    for para in paras or []:
        story.append(p(para))


def subsection(story: list, heading: str, paras: list[str] | None = None):
    story.append(p(heading, "h2"))
    for para in paras or []:
        story.append(p(para))


def minor(story: list, heading: str, paras: list[str] | None = None):
    story.append(p(heading, "h3"))
    for para in paras or []:
        story.append(p(para))


FEATURE_ROWS = [
    ["โหมด/คุณสมบัติ", "USART1/2/3", "UART4/5", "LPUART1", "หมายเหตุ"],
    ["Hardware flow control สำหรับ modem", "รองรับ", "รองรับ", "รองรับ", "ใช้ RTS/CTS"],
    ["สื่อสารต่อเนื่องด้วย DMA", "รองรับ", "รองรับ", "รองรับ", "แยก request Tx/Rx"],
    ["Multiprocessor communication", "รองรับ", "รองรับ", "รองรับ", "ใช้ mute mode และ address/idle wake-up"],
    ["Synchronous mode master/slave", "รองรับ", "ไม่รองรับ", "ไม่รองรับ", "ใช้ขา CK"],
    ["Smartcard mode", "รองรับ", "ไม่รองรับ", "ไม่รองรับ", "ISO 7816-3, T=0/T=1"],
    ["Single-wire half-duplex", "รองรับ", "รองรับ", "รองรับ", "ใช้ TX เป็นเส้นร่วม"],
    ["IrDA SIR ENDEC", "รองรับ", "รองรับ", "ไม่รองรับ", "ครึ่งดูเพล็กซ์"],
    ["LIN mode", "รองรับ", "รองรับ", "ไม่รองรับ", "break generation/detection"],
    ["Dual clock domain และ wake-up จาก low-power", "รองรับ", "รองรับ", "รองรับ", "ขึ้นกับ instance และ Stop mode"],
    ["Receiver timeout interrupt", "รองรับ", "รองรับ", "ไม่รองรับ", "ใช้ RTOEN/RTOIE/RTOF"],
    ["Modbus support", "รองรับ", "รองรับ", "ไม่รองรับ", "RTU timeout, ASCII CR/LF"],
    ["Auto baud rate detection", "รองรับ", "รองรับ", "ไม่รองรับ", "ใช้ ABREN/ABRMOD"],
    ["Driver Enable สำหรับ RS-485", "รองรับ", "รองรับ", "รองรับ", "ใช้ DEM/DEP/DEAT/DEDT"],
    ["ความยาวข้อมูล", "7, 8, 9 bit", "7, 8, 9 bit", "7, 8, 9 bit", "ตั้งด้วย M1/M0"],
    ["Tx/Rx FIFO", "รองรับ", "รองรับ", "รองรับ", "ขนาด FIFO = 8"],
    ["Wake-up จาก Stop 0/Stop 1", "รองรับ", "รองรับ", "รองรับ", "ตรวจจาก manual ของ instance นั้น"],
]


PIN_ROWS = [
    ["สัญญาณ", "ทิศทาง", "ความหมาย"],
    ["USART_TX / UART_TX", "output", "ส่งข้อมูลอนุกรมจาก transmitter"],
    ["USART_RX / UART_RX", "input", "รับข้อมูลอนุกรมเข้าสู่ receiver"],
    ["USART_CK", "I/O", "นาฬิกาสำหรับ synchronous mode; master ส่ง CK ออก, slave รับ CK เข้า"],
    ["USART_CTS / UART_CTS", "input", "Clear To Send สำหรับ hardware flow control; บาง instance ใช้ร่วมกับ NSS"],
    ["USART_RTS / UART_RTS", "output", "Request To Send สำหรับ hardware flow control; ใช้ร่วมกับ DE ได้ในบาง instance"],
    ["USART_DE / UART_DE", "output", "Driver Enable สำหรับควบคุม transceiver RS-485"],
    ["USART_NSS", "input", "Slave select ใน synchronous slave/SPI-like mode เมื่อเปิดใช้ NSS hardware"],
]


WORD_ROWS = [
    ["M[1:0]", "PCE", "รูปแบบเฟรม"],
    ["00", "0", "Start + 8 data bits + Stop"],
    ["00", "1", "Start + 7 data bits + parity bit + Stop"],
    ["01", "0", "Start + 9 data bits + Stop"],
    ["01", "1", "Start + 8 data bits + parity bit + Stop"],
    ["10", "0", "Start + 7 data bits + Stop"],
    ["10", "1", "Start + 6 data bits + parity bit + Stop"],
]


BAUD_EXAMPLE_ROWS = [
    ["กรณี", "การคำนวณ", "ค่า BRR"],
    ["9600 baud, usart_ker_ck_pres = 8 MHz, OVER8 = 0", "USARTDIV = 8,000,000 / 9600 = 833", "0x0341"],
    ["9600 baud, usart_ker_ck_pres = 8 MHz, OVER8 = 1", "USARTDIV = 2 x 8,000,000 / 9600 = 1667; BRR[3] = 0; เศษล่าง shift ขวา 1 bit", "0x0681"],
    ["921.6 kbaud, usart_ker_ck_pres = 48 MHz, OVER8 = 0", "USARTDIV = 48,000,000 / 921,600 = 52", "0x0034"],
    ["921.6 kbaud, usart_ker_ck_pres = 48 MHz, OVER8 = 1", "USARTDIV = 2 x 48,000,000 / 921,600 = 104; BRR[3:0] จาก 0x8 >> 1", "0x0064"],
]


TOLERANCE_ZERO_ROWS = [
    ["M bits", "OVER8=0, ONEBIT=0", "OVER8=0, ONEBIT=1", "OVER8=1, ONEBIT=0", "OVER8=1, ONEBIT=1"],
    ["00", "3.75%", "4.375%", "2.50%", "3.75%"],
    ["01", "3.41%", "3.97%", "2.27%", "3.41%"],
    ["10", "4.16%", "4.86%", "2.77%", "4.16%"],
]


TOLERANCE_NONZERO_ROWS = [
    ["M bits", "OVER8=0, ONEBIT=0", "OVER8=0, ONEBIT=1", "OVER8=1, ONEBIT=0", "OVER8=1, ONEBIT=1"],
    ["00", "3.33%", "3.88%", "2.00%", "3.00%"],
    ["01", "3.03%", "3.53%", "1.82%", "2.73%"],
    ["10", "3.70%", "4.31%", "2.22%", "3.33%"],
]


AUTO_BAUD_ROWS = [
    ["โหมด", "pattern ที่ใช้", "วิธีวัด"],
    ["Mode 0", "อักขระใดก็ได้ที่เริ่มด้วย bit = 1", "วัดความยาว start bit จาก falling edge ถึง rising edge"],
    ["Mode 1", "อักขระที่เริ่มด้วย pattern 10xx", "วัดช่วง start bit และ data bit แรก จาก falling edge ถึง falling edge เพื่อความแม่นเมื่อขอบสัญญาณช้า"],
    ["Mode 2", "เฟรม 0x7F ใน LSB-first หรือ 0xFE ใน MSB-first", "อัปเดต baud หลัง start bit และหลัง bit 6"],
    ["Mode 3", "เฟรม 0x55", "อัปเดต baud หลัง start bit, bit 0 และ bit 6 พร้อมตรวจ transition ระหว่างทาง"],
]


INTERRUPT_ROWS = [
    ["เหตุการณ์", "Flag", "Enable bit", "วิธี clear/จัดการ", "ปลุกจาก low-power"],
    ["Transmit data register empty", "TXE", "TXEIE", "เขียน TDR", "Sleep"],
    ["Transmit FIFO not full", "TXFNF", "TXFNFIE", "เติม TXFIFO จนเต็ม", "Sleep"],
    ["Transmit FIFO empty", "TXFE", "TXFEIE", "เขียน TDR หรือ TXFRQ", "Sleep"],
    ["TXFIFO threshold reached", "TXFT", "TXFTIE", "เขียน TDR", "Sleep"],
    ["CTS interrupt", "CTSIF", "CTSIE", "เขียน CTSCF = 1", "Sleep"],
    ["Transmission complete", "TC", "TCIE", "เขียน TDR หรือ TCCF = 1", "Sleep"],
    ["Transmission complete before guard time", "TCBGT", "TCBGTIE", "เขียน TDR หรือ TCBGTCF = 1", "Sleep"],
    ["Receive data register not empty", "RXNE", "RXNEIE", "อ่าน RDR หรือ RXFRQ = 1", "Sleep, Stop"],
    ["Receive FIFO not empty", "RXFNE", "RXFNEIE", "อ่าน RDR จน FIFO ว่าง หรือ RXFRQ = 1", "Sleep, Stop"],
    ["Receive FIFO full", "RXFF", "RXFFIE", "อ่าน RDR", "Sleep, Stop"],
    ["RXFIFO threshold reached", "RXFT", "RXFTIE", "อ่าน RDR", "Sleep, Stop"],
    ["Overrun error", "ORE", "RXNEIE/RXFNEIE หรือ EIE", "เขียน ORECF = 1", "Sleep"],
    ["Idle line detected", "IDLE", "IDLEIE", "เขียน IDLECF = 1", "Sleep"],
    ["Parity error", "PE", "PEIE", "เขียน PECF = 1", "Sleep"],
    ["LIN break", "LBDF", "LBDIE", "เขียน LBDCF = 1", "Sleep"],
    ["Noise/framing/overrun ใน multibuffer", "NE/FE/ORE", "EIE", "เขียน NECF/FECF/ORECF = 1", "Sleep"],
    ["Character match", "CMF", "CMIE", "เขียน CMCF = 1", "Sleep"],
    ["Receiver timeout", "RTOF", "RTOIE", "เขียน RTOCF = 1", "Sleep"],
    ["End of block", "EOBF", "EOBIE", "เขียน EOBCF = 1", "Sleep"],
    ["Wake-up from low-power", "WUF", "WUFIE", "เขียน WUCF = 1", "Stop ตาม instance"],
    ["SPI slave underrun", "UDR", "EIE", "เขียน UDRCF = 1", "Sleep"],
]


LOW_POWER_ROWS = [
    ["Mode", "ผลต่อ USART"],
    ["Sleep", "ไม่มีผลกับการทำงานของ USART; interrupt ของ USART ทำให้ MCU ออกจาก Sleep ได้"],
    ["Stop", "ค่า register ยังอยู่ ถ้า USART clock มาจาก oscillator ที่ยังทำงานใน Stop และ instance รองรับ ก็สามารถปลุก MCU จาก Stop ได้"],
    ["Standby", "USART ถูก power down ต้อง init ใหม่หลังออกจาก Standby"],
]


CR1_ROWS = [
    ["Bit", "ชื่อ", "คำอธิบายภาษาไทย"],
    ["31", "RXFFIE", "เปิด interrupt เมื่อ RXFIFO full และ RXFF = 1"],
    ["30", "TXFEIE", "เปิด interrupt เมื่อ TXFIFO empty และ TXFE = 1"],
    ["29", "FIFOEN", "เปิด/ปิด FIFO mode; เขียนได้เมื่อ UE = 0 เท่านั้น; ห้ามใช้ใน IrDA และ LIN"],
    ["28", "M1", "กำหนด word length ร่วมกับ M0: 00=8 data, 01=9 data, 10=7 data; เขียนได้เมื่อ UE = 0"],
    ["27", "EOBIE", "เปิด interrupt เมื่อ EOBF set; ใช้กับ smartcard/block mode เมื่อรองรับ"],
    ["26", "RTOIE", "เปิด interrupt เมื่อ RTOF set; ใช้ receiver timeout เมื่อรองรับ"],
    ["25:21", "DEAT[4:0]", "เวลาหน่วงจาก DE active ถึง start bit หน่วยเป็น sample time 1/8 หรือ 1/16 bit time"],
    ["20:16", "DEDT[4:0]", "เวลาหน่วงจาก stop bit สุดท้ายถึง DE inactive หน่วยเป็น sample time"],
    ["15", "OVER8", "0=oversampling by 16, 1=oversampling by 8; ต้องเป็น 0 ใน LIN, IrDA, smartcard"],
    ["14", "CMIE", "เปิด interrupt เมื่อ character match flag CMF set"],
    ["13", "MME", "เปิด mute mode; receiver สลับ active/mute ตามวิธี WAKE"],
    ["12", "M0", "กำหนด word length ร่วมกับ M1"],
    ["11", "WAKE", "วิธีปลุกจาก mute: 0=idle line, 1=address mark"],
    ["10", "PCE", "เปิด parity generation/checking; parity ถูกใส่ที่ MSB ของ word"],
    ["9", "PS", "เลือก parity: 0=even, 1=odd"],
    ["8", "PEIE", "เปิด interrupt เมื่อ PE = 1"],
    ["7", "TXFNFIE", "FIFO enabled: เปิด interrupt เมื่อ TXFIFO not full"],
    ["6", "TCIE", "เปิด interrupt เมื่อ transmission complete TC = 1"],
    ["5", "RXFNEIE", "FIFO enabled: เปิด interrupt เมื่อ RXFIFO not empty หรือ ORE"],
    ["4", "IDLEIE", "เปิด interrupt เมื่อพบ idle line"],
    ["3", "TE", "เปิด transmitter; ตั้ง TE แล้วจะส่ง idle frame ก่อนเริ่มส่งจริงในโหมดปกติ"],
    ["2", "RE", "เปิด receiver และเริ่มค้นหา start bit"],
    ["1", "UESM", "อนุญาตให้ USART ปลุก MCU จาก low-power mode; ควร set ก่อนเข้า low-power และ clear หลังออก"],
    ["0", "UE", "เปิด USART; เมื่อ clear จะหยุด prescaler/output ทันทีและ reset status flags"],
]


CR1_ALT_ROWS = [
    ["Bit", "ชื่อ", "ความต่างเมื่อ FIFO disabled"],
    ["31:30", "Reserved", "ต้องคง reset value"],
    ["29", "FIFOEN", "ยังเป็นบิตเปิด FIFO เช่นเดิม เขียนได้เมื่อ UE = 0"],
    ["7", "TXEIE", "แทน TXFNFIE; เปิด interrupt เมื่อ transmit data register empty"],
    ["5", "RXNEIE", "แทน RXFNEIE; เปิด interrupt เมื่อ receive data register not empty หรือ ORE"],
    ["อื่น ๆ", "เหมือน CR1", "ความหมายเดียวกับตาราง FIFO enabled"],
]


CR2_ROWS = [
    ["Bit", "ชื่อ", "คำอธิบายภาษาไทย"],
    ["31:24", "ADD[7:0]", "address ของ USART node ใช้กับ multiprocessor address match; 4-bit/7-bit detection เลือกด้วย ADDM7"],
    ["23", "RTOEN", "เปิด receiver timeout"],
    ["22:21", "ABRMOD[1:0]", "เลือก mode ของ auto baud rate detection"],
    ["20", "ABREN", "เปิด auto baud rate detection"],
    ["19", "MSBFIRST", "เลือกส่ง/รับ MSB ก่อน แทนค่า default LSB-first"],
    ["18", "DATAINV", "กลับค่าข้อมูล binary ภายใน data frame"],
    ["17", "TXINV", "กลับ polarity ของ TX pin"],
    ["16", "RXINV", "กลับ polarity ของ RX pin"],
    ["15", "SWAP", "สลับหน้าที่ขา TX/RX"],
    ["14", "LINEN", "เปิด LIN mode; ต้องปิด STOP/CLKEN และบางโหมดใน CR3 ตามเงื่อนไข"],
    ["13:12", "STOP[1:0]", "เลือก stop bits: โหมดปกติรองรับ 1 หรือ 2; smartcard รองรับ 0.5/1.5"],
    ["11", "CLKEN", "เปิด clock output/input บนขา CK สำหรับ synchronous/smartcard clock"],
    ["10", "CPOL", "clock polarity ใน synchronous mode"],
    ["9", "CPHA", "clock phase ใน synchronous mode"],
    ["8", "LBCL", "ควบคุม clock pulse ของ data bit สุดท้ายใน synchronous master"],
    ["7", "Reserved", "ต้องคง reset value"],
    ["6", "LBDIE", "เปิด interrupt เมื่อ LIN break detected"],
    ["5", "LBDL", "เลือกความยาว LIN break detection 10 หรือ 11 bit"],
    ["4", "ADDM7", "เลือก 7-bit address detection แทน 4-bit address detection"],
    ["3", "DIS_NSS", "ควบคุมการใช้ NSS pin ใน synchronous slave; 1=software NSS"],
    ["2:1", "Reserved", "ต้องคง reset value"],
    ["0", "SLVEN", "เปิด synchronous slave mode"],
]


CR3_ROWS = [
    ["Bit", "ชื่อ", "คำอธิบายภาษาไทย"],
    ["31:29", "TXFTCFG[2:0]", "กำหนด threshold ของ TXFIFO"],
    ["28", "RXFTIE", "เปิด interrupt เมื่อ RXFIFO ถึง threshold"],
    ["27:25", "RXFTCFG[2:0]", "กำหนด threshold ของ RXFIFO"],
    ["24", "TCBGTIE", "เปิด interrupt เมื่อ transmission complete before guard time"],
    ["23", "TXFTIE", "เปิด interrupt เมื่อ TXFIFO ถึง threshold"],
    ["22", "WUFIE", "เปิด interrupt wake-up from low-power"],
    ["21:20", "WUS[1:0]", "เลือก event สำหรับ wake-up จาก low-power"],
    ["19:17", "SCARCNT[2:0]", "จำนวน retry อัตโนมัติใน smartcard เมื่อเกิด NACK/parity error"],
    ["16", "Reserved", "ต้องคง reset value"],
    ["15", "DEP", "เลือก polarity ของ DE signal"],
    ["14", "DEM", "เปิด Driver Enable mode สำหรับ RS-485"],
    ["13", "DDRE", "ปิด DMA reception เมื่อเกิด reception error"],
    ["12", "OVRDIS", "ปิดการตรวจ overrun error"],
    ["11", "ONEBIT", "ใช้ one-sample method; เพิ่ม tolerance แต่ลดการตรวจ noise"],
    ["10", "CTSIE", "เปิด interrupt เมื่อ CTSIF set"],
    ["9", "CTSE", "เปิด CTS hardware flow control"],
    ["8", "RTSE", "เปิด RTS hardware flow control"],
    ["7", "DMAT", "เปิด DMA สำหรับ transmit"],
    ["6", "DMAR", "เปิด DMA สำหรับ receive"],
    ["5", "SCEN", "เปิด smartcard mode"],
    ["4", "NACK", "เปิด NACK ใน smartcard reception เมื่อ parity error"],
    ["3", "HDSEL", "เลือก single-wire half-duplex"],
    ["2", "IRLP", "เลือก IrDA low-power mode"],
    ["1", "IREN", "เปิด IrDA mode"],
    ["0", "EIE", "เปิด error interrupt สำหรับ FE/NE/ORE และ UDR ในบางโหมด"],
]


BRR_ROWS = [
    ["Register", "Bit", "ชื่อ", "คำอธิบายภาษาไทย"],
    ["USART_BRR", "15:0", "BRR[15:0]", "ค่า fixed-point ของ USARTDIV สำหรับ baud-rate generator; ห้ามเปลี่ยนระหว่างสื่อสาร"],
    ["USART_GTPR", "15:8", "GT[7:0]", "guard time value ใช้หลัก ๆ ใน smartcard"],
    ["USART_GTPR", "7:0", "PSC[7:0]", "prescaler value สำหรับ smartcard clock/IrDA low-power; IrDA ENDEC ใช้ไม่ได้เมื่อ PSC = 0"],
    ["USART_RTOR", "31:24", "BLEN[7:0]", "block length สำหรับ smartcard T=1; end of block แจ้งด้วย EOBF"],
    ["USART_RTOR", "23:0", "RTO[23:0]", "receiver timeout value หน่วยเป็น baud time"],
]


RQR_ROWS = [
    ["Bit", "ชื่อ", "คำอธิบายภาษาไทย"],
    ["31:5", "Reserved", "ต้องคง reset value"],
    ["4", "TXFRQ", "request flush ข้อมูลส่ง; ใช้ทำให้ TXE/TXFNF set ในบางกรณี"],
    ["3", "RXFRQ", "request flush ข้อมูลรับ; clear RXNE/RXFNE"],
    ["2", "MMRQ", "request เข้า mute mode"],
    ["1", "SBKRQ", "request ส่ง break"],
    ["0", "ABRRQ", "request auto baud rate ใหม่"],
]


ISR_FIFO_ROWS = [
    ["Bit", "ชื่อ", "คำอธิบายภาษาไทย"],
    ["31:28", "Reserved", "ต้องคง reset value"],
    ["27", "TXFT", "TXFIFO ถึง threshold"],
    ["26", "RXFT", "RXFIFO ถึง threshold"],
    ["25", "TCBGT", "ส่งเสร็จก่อน guard time"],
    ["24", "RXFF", "RXFIFO full"],
    ["23", "TXFE", "TXFIFO empty"],
    ["22", "REACK", "receiver enable acknowledge"],
    ["21", "TEACK", "transmitter enable acknowledge"],
    ["20", "WUF", "wake-up from low-power flag"],
    ["19", "RWU", "receiver อยู่ใน mute mode"],
    ["18", "SBKF", "send break flag"],
    ["17", "CMF", "character match flag"],
    ["16", "BUSY", "USART กำลังรับ/ส่งหรือ line ยังไม่ idle"],
    ["15", "ABRF", "auto baud rate complete"],
    ["14", "ABRE", "auto baud rate error"],
    ["13", "UDR", "SPI slave underrun error"],
    ["12", "EOBF", "end of block flag"],
    ["11", "RTOF", "receiver timeout"],
    ["10", "CTS", "สถานะ CTS"],
    ["9", "CTSIF", "CTS interrupt flag"],
    ["8", "LBDF", "LIN break detected"],
    ["7", "TXFNF", "TXFIFO not full"],
    ["6", "TC", "transmission complete"],
    ["5", "RXFNE", "RXFIFO not empty"],
    ["4", "IDLE", "ตรวจพบ idle line"],
    ["3", "ORE", "overrun error"],
    ["2", "NE", "noise detected"],
    ["1", "FE", "framing error"],
    ["0", "PE", "parity error"],
]


ISR_ALT_ROWS = [
    ["Bit", "ชื่อ", "ความหมายเมื่อ FIFO disabled"],
    ["31:26", "Reserved", "ต้องคง reset value"],
    ["25", "TCBGT", "transmission complete before guard time"],
    ["24:23", "Reserved", "ไม่มี RXFF/TXFE ใน alternate map"],
    ["22:8", "เหมือน ISR", "REACK, TEACK, WUF, RWU, SBKF, CMF, BUSY, ABRF, ABRE, UDR, EOBF, RTOF, CTS, CTSIF, LBDF"],
    ["7", "TXE", "transmit data register empty"],
    ["6", "TC", "transmission complete"],
    ["5", "RXNE", "read data register not empty"],
    ["4:0", "IDLE/ORE/NE/FE/PE", "flag เหมือน FIFO enabled"],
]


ICR_ROWS = [
    ["Bit", "ชื่อ", "เขียน 1 เพื่อ clear"],
    ["20", "WUCF", "clear WUF"],
    ["17", "CMCF", "clear CMF"],
    ["13", "UDRCF", "clear UDR"],
    ["12", "EOBCF", "clear EOBF"],
    ["11", "RTOCF", "clear RTOF"],
    ["9", "CTSCF", "clear CTSIF"],
    ["8", "LBDCF", "clear LBDF"],
    ["7", "TCBGTCF", "clear TCBGT"],
    ["6", "TCCF", "clear TC"],
    ["5", "TXFECF", "clear TXFE"],
    ["4", "IDLECF", "clear IDLE"],
    ["3", "ORECF", "clear ORE"],
    ["2", "NECF", "clear NE"],
    ["1", "FECF", "clear FE"],
    ["0", "PECF", "clear PE"],
]


DATA_ROWS = [
    ["Register", "Bit", "ชื่อ", "คำอธิบายภาษาไทย"],
    ["USART_RDR", "8:0", "RDR[8:0]", "ค่าข้อมูลรับ อ่านแล้วช่วย clear RXNE/RXFNE ตามโหมด"],
    ["USART_TDR", "8:0", "TDR[8:0]", "ค่าข้อมูลส่ง เขียนเพื่อส่ง frame ถัดไป"],
    ["USART_PRESC", "3:0", "PRESCALER[3:0]", "clock prescaler ก่อนเข้า baud-rate generator"],
]


REGISTER_MAP_ROWS = [
    ["Offset", "Register", "หน้าที่หลัก"],
    ["0x00", "USART_CR1", "เปิด peripheral, Tx/Rx, word length, parity, FIFO, interrupt หลัก, RS-485 DE timing"],
    ["0x04", "USART_CR2", "stop bits, LIN, synchronous clock, auto baud, address, inversion, swap pin, slave mode"],
    ["0x08", "USART_CR3", "DMA, hardware flow control, smartcard, IrDA, half-duplex, FIFO threshold, wake-up"],
    ["0x0C", "USART_BRR", "baud-rate divider"],
    ["0x10", "USART_GTPR", "guard time และ prescaler"],
    ["0x14", "USART_RTOR", "receiver timeout และ block length"],
    ["0x18", "USART_RQR", "request register สำหรับ flush, mute, break, auto-baud request"],
    ["0x1C", "USART_ISR", "interrupt/status flags"],
    ["0x20", "USART_ICR", "interrupt flag clear register"],
    ["0x24", "USART_RDR", "receive data"],
    ["0x28", "USART_TDR", "transmit data"],
    ["0x2C", "USART_PRESC", "clock prescaler"],
]


def add_cover(story: list):
    story.append(Spacer(1, 28 * mm))
    story.append(p("RM0440 Reference Manual STM32G4", "title"))
    story.append(p("บทที่ 40 USART/UART - ฉบับภาษาไทย", "title"))
    story.append(p("แปลและเรียบเรียงจาก RM0440_ReferenceManual_STM32G4.pdf หน้า 1689-1774", "subtitle"))
    story.append(Spacer(1, 10 * mm))
    story.append(
        p(
            "เอกสารนี้จัดทำเป็นคู่มือภาษาไทยสำหรับอ่านและอ้างอิงการตั้งค่า USART/UART บน STM32G4 "
            "โดยคงชื่อ register, bit field, flag, signal และสมการตามเอกสารต้นฉบับ เพื่อให้เทียบกับโค้ด HAL/LL หรือ bare-metal ได้ตรงกัน.",
            "cover_note",
        )
    )
    story.append(Spacer(1, 8 * mm))
    story.append(p("ขอบเขต", "h2"))
    story.extend(
        bullets(
            [
                "ครอบคลุมหัวข้อ 40.1 ถึง 40.8.15: ภาพรวม, feature, functional description, low-power, interrupt และ register map.",
                "ตารางและภาพจากต้นฉบับถูกแปลงเป็นคำอธิบาย/ตารางภาษาไทยเพื่อให้อ่านใน PDF ใหม่ได้สะดวกขึ้น.",
                "ชื่อเฉพาะเช่น USART_CR1, RXFNE, DMAT, ABRMOD, SCARCNT และ PRESCALER ยังคงเป็นภาษาอังกฤษตาม manual.",
            ]
        )
    )
    story.append(PageBreak())


def add_intro(story: list):
    section(
        story,
        "40.1 USART introduction",
        [
            "USART เป็น peripheral สำหรับสื่อสารอนุกรมแบบ synchronous/asynchronous ที่ยืดหยุ่น ใช้แลกเปลี่ยนข้อมูลแบบ full-duplex กับอุปกรณ์ภายนอกที่ใช้รูปแบบ NRZ asynchronous serial มาตรฐานได้ ช่วง baud rate กว้างมากเพราะมี fractional baud-rate generator.",
            "นอกจาก UART ปกติแล้ว USART ยังรองรับ synchronous one-way, single-wire half-duplex, LIN, smartcard protocol, IrDA SIR ENDEC, modem control ด้วย CTS/RTS และ multiprocessor communication ได้ การสื่อสารความเร็วสูงหรือแบบต่อเนื่องทำได้ด้วย DMA และ multibuffer configuration.",
        ],
    )
    section(story, "40.2 USART main features")
    story.extend(
        bullets(
            [
                "สื่อสาร asynchronous แบบ full-duplex ด้วย NRZ mark/space format.",
                "เลือก oversampling by 16 หรือ by 8 เพื่อแลกระหว่างความเร็วกับ clock tolerance.",
                "ตั้ง baud rate ได้ด้วย baud-rate generator และใช้ baud rate ร่วมกันทั้ง Tx/Rx.",
                "มี Tx FIFO และ Rx FIFO ขนาด 8 entry ซึ่งเปิด/ปิดด้วย software และมี status flag.",
                "มี dedicated kernel clock แยกจาก PCLK ทำให้ peripheral clock domain ยืดหยุ่นขึ้น.",
                "รองรับ auto baud rate detection, data word length 7/8/9 bits และเลือก MSB-first หรือ LSB-first ได้.",
                "ตั้ง stop bits ได้ 1 หรือ 2 bits ในโหมดปกติ.",
                "รองรับ synchronous master/slave, single-wire half-duplex, DMA continuous communication, hardware flow control, RS-485 driver enable และ pin swap.",
                "มี parity generation/checking, communication/error flags, interrupt sources, multiprocessor wake-up และ wake-up from Stop mode ตาม instance ที่รองรับ.",
            ]
        )
    )
    section(story, "40.3 USART extended features")
    story.extend(
        bullets(
            [
                "LIN: master ส่ง synchronous break ได้ และ slave ตรวจ break ได้ โดยรองรับ 13-bit break generation และ 10/11-bit break detection เมื่อเปิด LIN hardware.",
                "IrDA: มี SIR encoder/decoder สำหรับ normal mode pulse width 3/16 bit duration.",
                "Smartcard: รองรับ asynchronous protocol T=0 และ T=1 ตาม ISO/IEC 7816-3, รวมถึง 0.5 และ 1.5 stop bits.",
                "Modbus: มี timeout feature และ character recognition สำหรับ CR/LF เพื่อช่วย Modbus/RTU และ Modbus/ASCII.",
            ]
        )
    )
    section(story, "40.4 USART implementation")
    story.append(tbl(FEATURE_ROWS, [0.32, 0.15, 0.13, 0.13, 0.27], tiny=True))
    story.append(p("หมายเหตุ: X หรือคำว่า 'รองรับ' หมายถึง peripheral instance นั้นมี feature ดังกล่าว ส่วน wake-up from Stop mode ต้องดูข้อจำกัดของ instance และ Stop mode ที่ใช้.", "note"))


def add_functional(story: list):
    section(story, "40.5 USART functional description")
    subsection(
        story,
        "40.5.1 USART block diagram",
        [
            "โครงสร้างภายในประกอบด้วยสายทาง transmitter/receiver, shift register, data register/FIFO, baud-rate generator, clock/prescaler, control register, status/interrupt logic, DMA request และ wake-up logic. เส้นทางรับและส่งแยกกัน จึงทำ full-duplex ได้เมื่อใช้ TX และ RX แยกขา.",
            "เมื่อเปิด FIFOEN ข้อมูลส่งจะเข้าคิว TXFIFO ก่อนออก shift register และข้อมูลรับจะเข้าคิว RXFIFO ก่อนอ่านผ่าน RDR ถ้าปิด FIFO พฤติกรรมจะกลับไปใช้ register เดี่ยวพร้อม flag TXE/RXNE.",
        ],
    )
    subsection(story, "40.5.2 USART signals")
    story.append(tbl(PIN_ROWS, [0.22, 0.14, 0.64], tiny=True))
    subsection(
        story,
        "40.5.3 USART character description",
        [
            "เฟรม asynchronous ปกติเริ่มจาก line idle เป็น high, start bit เป็น low, ตามด้วย data bits, parity bit ถ้าเปิด PCE และ stop bit ตามค่าที่ตั้งใน STOP[1:0]. ค่า default ส่ง LSB-first แต่สามารถเลือก MSB-first ด้วย MSBFIRST.",
            "บิต M1/M0 ใน USART_CR1 กำหนด word length เมื่อเปิด parity แล้ว parity จะกินตำแหน่ง MSB ของ word จึงทำให้จำนวน payload data ลดลงหนึ่ง bit.",
        ],
    )
    story.append(tbl(WORD_ROWS, [0.16, 0.12, 0.72], tiny=True))
    subsection(
        story,
        "40.5.4 USART FIFOs and thresholds",
        [
            "FIFO mode เปิดด้วย FIFOEN ใน USART_CR1 และต้องเขียนเมื่อ UE = 0 เท่านั้น แต่ละทิศทางมี FIFO ขนาด 8 entry. ใน FIFO mode flag สำคัญคือ RXFNE, RXFF, RXFT, TXFNF, TXFE และ TXFT.",
            "RXFTCFG/TXFTCFG ใน USART_CR3 เลือกระดับ threshold สำหรับ interrupt หรือ DMA strategy ได้ ส่วน RXFTIE/TXFTIE เปิด interrupt เมื่อถึง threshold. เมื่อใช้ low-power mode threshold เหล่านี้ช่วยให้ MCU ตื่นเมื่อมีข้อมูลมากพอหรือต้องเติม TXFIFO.",
        ],
    )
    subsection(story, "40.5.5 USART transmitter")
    story.extend(
        bullets(
            [
                "กำหนด word length ด้วย M1/M0 ก่อนเปิด USART.",
                "ตั้ง baud rate ใน USART_BRR และตั้ง stop bits ใน USART_CR2.",
                "เขียน UE = 1 เพื่อเปิด USART.",
                "ถ้าต้องใช้ DMA ให้ตั้ง DMAT ใน USART_CR3.",
                "ตั้ง TE = 1 เพื่อเปิด transmitter; ในโหมดปกติจะส่ง idle frame เป็นการเริ่มต้น.",
                "เขียนข้อมูลลง USART_TDR เมื่อ TXE/TXFNF พร้อม และทำซ้ำจนส่งครบ.",
                "หลังเขียนข้อมูลสุดท้ายแล้วรอ TC = 1 เพื่อให้แน่ใจว่า frame สุดท้ายออกจาก shift register แล้ว ก่อนปิด USART หรือเข้า low-power.",
            ]
        )
    )
    story.append(p("ใน FIFO mode, TXFNF หมายถึง TXFIFO ยังไม่เต็ม ส่วน TXFE หมายถึง TXFIFO ว่างทั้งหมด. TC จะ set เมื่อการส่งออกทางสายจริงเสร็จ ไม่ใช่แค่เขียน TDR เสร็จ.", "note"))
    subsection(story, "40.5.6 USART receiver")
    story.extend(
        bullets(
            [
                "กำหนด M1/M0, baud rate และ stop bits ก่อนเปิด receiver.",
                "เขียน UE = 1 แล้วเปิด DMA ด้วย DMAR ถ้าต้องการ multibuffer reception.",
                "ตั้ง RE = 1 เพื่อเริ่มค้นหา start bit.",
                "เมื่อรับข้อมูลได้ RXNE/RXFNE จะ set; อ่าน USART_RDR เพื่อนำข้อมูลออก.",
                "ตรวจ PE, FE, NE และ ORE เพื่อจัดการ parity error, framing error, noise และ overrun.",
            ]
        )
    )
    story.append(
        p(
            "Oversampling by 16 ให้ margin ต่อ noise ดี ส่วน oversampling by 8 ช่วยเพิ่ม baud สูงสุดแต่ tolerance ต่อ clock deviation ลดลง. ONEBIT = 1 ใช้วิธี sample จุดเดียว เพิ่ม tolerance บางกรณีแต่ลดความสามารถตรวจ noise.",
            "note",
        )
    )
    subsection(story, "40.5.7 USART baud rate generation")
    story.extend(
        bullets(
            [
                "Rx และ Tx ใช้ baud rate จาก USART_BRR ค่าเดียวกัน.",
                "โหมด USART ปกติรวม SPI-like synchronous: ถ้า OVER8 = 0, baud = usart_ker_ck_pres / USARTDIV.",
                "ถ้า OVER8 = 1, baud = 2 x usart_ker_ck_pres / USARTDIV.",
                "ใน smartcard, LIN และ IrDA ใช้ OVER8 = 0 และ baud = usart_ker_ck_pres / USARTDIV.",
                "USARTDIV ถูกเข้ารหัสใน USART_BRR; เมื่อ OVER8 = 0 ให้ BRR = USARTDIV. เมื่อ OVER8 = 1, BRR[3] ต้องเป็น 0, BRR[2:0] คือ USARTDIV[3:0] shift ขวา 1 bit และ BRR[15:4] = USARTDIV[15:4].",
                "ไม่ควรเปลี่ยน USART_BRR ระหว่างสื่อสาร และ USARTDIV ต้องมากกว่าหรือเท่ากับ 16 สำหรับ oversampling by 16/8 ตามเงื่อนไข manual.",
            ]
        )
    )
    story.append(tbl(BAUD_EXAMPLE_ROWS, [0.32, 0.48, 0.20], tiny=True))
    subsection(story, "40.5.8 Receiver tolerance ต่อ clock deviation")
    story.append(
        p(
            "Receiver asynchronous ทำงานถูกต้องเมื่อผลรวมความคลาดเคลื่อนของระบบน้อยกว่า tolerance ของ receiver: DTRA + DQUANT + DREC + DTCL + DWU < USART receiver tolerance. ค่าเหล่านี้มาจากความคลาดของ transmitter, quantization ของ baud receiver, oscillator ฝั่งรับ, line/transceiver asymmetry และ wake-up delay.",
            "normal",
        )
    )
    minor(story, "กรณี BRR[3:0] = 0000")
    story.append(tbl(TOLERANCE_ZERO_ROWS, [0.16, 0.21, 0.21, 0.21, 0.21], tiny=True))
    minor(story, "กรณี BRR[3:0] ไม่เท่ากับ 0000")
    story.append(tbl(TOLERANCE_NONZERO_ROWS, [0.16, 0.21, 0.21, 0.21, 0.21], tiny=True))
    subsection(story, "40.5.9 USART auto baud rate detection")
    story.append(
        p(
            "USART สามารถวัด baud จากอักขระแรกที่รับและตั้งค่า USART_BRR ให้อัตโนมัติ เหมาะเมื่อไม่ทราบความเร็วล่วงหน้า หรือใช้ clock source ที่ความแม่นไม่สูงพอ. ก่อนเปิด ABREN ต้องเลือก ABRMOD[1:0] และต้องเขียนค่า baud ที่ไม่ใช่ศูนย์ลง BRR ไว้ก่อน.",
        )
    )
    story.append(tbl(AUTO_BAUD_ROWS, [0.14, 0.36, 0.50], tiny=True))
    story.append(
        p(
            "เมื่อสำเร็จ ABRF จะ set ใน USART_ISR. ถ้า line มี noise หรือ baud อยู่นอกช่วงที่รองรับ ค่า BRR อาจเสียและ ABRE จะ set. ถ้าเปิด FIFO ให้ตรวจว่า RXFIFO ว่างก่อนเริ่ม auto baud เพราะการวัดควรใช้ข้อมูลตำแหน่งแรกของ RXFIFO.",
            "note",
        )
    )
    subsection(story, "40.5.10 Multiprocessor communication")
    story.extend(
        bullets(
            [
                "ใช้หลาย USART บน bus เดียวได้ เช่น master ต่อ TX ไปยัง RX ของ slave หลายตัว และ TX ของ slave รวมกลับด้วยวงจรที่เหมาะสม.",
                "Mute mode ลดภาระ interrupt ของ node ที่ไม่ถูก address โดยตั้ง MME = 1.",
                "เมื่อ mute mode ทำงาน receive status bits และ receive interrupts จะไม่ set และ RWU = 1.",
                "WAKE = 0 ใช้ idle line detection เพื่อออกจาก mute; WAKE = 1 ใช้ address mark detection.",
                "Address mark: byte ที่ MSB = 1 ถือเป็น address และเทียบกับ ADD[7:0]/ADD[3:0] ตาม ADDM7. ถ้า address ตรงกัน RWU ถูก clear และรับ byte ถัดไปตามปกติ.",
            ]
        )
    )
    subsection(story, "40.5.11 Modbus communication")
    story.extend(
        bullets(
            [
                "Modbus/RTU ตรวจจบ block จากช่วง silence บน line มากกว่า 2 character times โดยใช้ receiver timeout. ตั้ง RTOEN, RTOIE และค่า RTO เช่น 22 x bit time.",
                "Modbus/ASCII ตรวจจบ block ด้วย CR/LF โดยใช้ character match: ใส่ ASCII LF ใน ADD[7:0] และเปิด CMIE จากนั้น software ตรวจ CR/LF ใน DMA buffer.",
                "USART ช่วยแค่กลไก timing/character match ส่วน address recognition, integrity check และ command interpretation ต้องทำใน software.",
            ]
        )
    )
    subsection(story, "40.5.12 Parity control")
    story.extend(
        bullets(
            [
                "เปิด parity ด้วย PCE และเลือก even/odd ด้วย PS.",
                "Even parity ทำให้จำนวน bit 1 ใน payload รวม parity เป็นเลขคู่; odd parity ทำให้เป็นเลขคี่.",
                "เมื่อรับแล้ว parity ผิด PE จะ set และถ้า PEIE = 1 จะเกิด interrupt. clear ด้วย PECF ใน USART_ICR.",
                "เมื่อส่งและ PCE = 1, bit MSB ที่ software เขียนใน data register จะถูกแทนด้วย parity bit ที่ hardware คำนวณ.",
            ]
        )
    )
    subsection(story, "40.5.13 LIN mode")
    story.extend(
        bullets(
            [
                "เปิด LIN ด้วย LINEN ใน USART_CR2. เมื่อใช้ LIN ต้อง clear STOP[1:0] และ CLKEN ใน CR2 รวมถึง SCEN, HDSEL และ IREN ใน CR3.",
                "LIN master transmission ใช้ขั้นตอนส่ง USART ปกติ แต่ตั้ง word length เป็น 8-bit และใช้ SBKRQ เพื่อส่ง break 13 bit 0 ตามด้วย bit 1 สอง bit สำหรับ start detection ถัดไป.",
                "LIN reception เปิดวงจร break detection แยกจาก receiver ปกติ. ถ้าพบ 10 หรือ 11 bit 0 ติดต่อกันตาม LBDL และตามด้วย delimiter, LBDF จะ set และ interrupt เกิดเมื่อ LBDIE = 1.",
                "ถ้า LINEN = 0 receiver ทำงานเป็น USART ปกติและไม่สน break detection.",
            ]
        )
    )
    subsection(story, "40.5.14 Synchronous mode")
    story.extend(
        bullets(
            [
                "Master mode เปิดด้วย CLKEN = 1. ต้อง clear LINEN, SCEN, HDSEL และ IREN. ขา CK เป็น clock output ของ transmitter.",
                "CPOL และ CPHA เลือก polarity/phase ของ CK. LBCL กำหนดว่าจะมี clock pulse ใน data bit สุดท้ายหรือไม่.",
                "ใน master mode จะรับ synchronous data ได้เฉพาะเมื่อมีการส่ง เพราะ CK ออกเมื่อ TE = 1 และมีข้อมูลใน TDR.",
                "Slave mode เปิดด้วย SLVEN = 1. ขา CK เป็น input และ usart_ker_ck_pres ต้องมากกว่า 3 เท่าของความถี่ CK input เมื่อใช้แบบ SPI slave.",
                "ใน slave transmission มี UDR flag เมื่อนาฬิกาสำหรับส่งเริ่มมาแต่ software ยังไม่ได้ใส่ข้อมูลใน TDR.",
                "NSS เลือก hardware/software management ด้วย DIS_NSS.",
            ]
        )
    )
    subsection(story, "40.5.15 Single-wire half-duplex")
    story.extend(
        bullets(
            [
                "เปิดด้วย HDSEL ใน USART_CR3 และต้อง clear LINEN, CLKEN, SCEN และ IREN.",
                "TX และ RX ถูกต่อกันภายใน; RX pin ไม่ถูกใช้.",
                "เมื่อไม่ส่งข้อมูล TX pin ถูก release จึงควรตั้งเป็น alternate function open-drain พร้อม external pull-up.",
                "Hardware ไม่กัน collision บน line ให้ ต้องจัดการด้วย software หรือ protocol/arbitration ภายนอก.",
            ]
        )
    )
    subsection(story, "40.5.16 Receiver timeout")
    story.extend(
        bullets(
            [
                "เปิดด้วย RTOEN ใน USART_CR2 และตั้งเวลาใน RTO[23:0] ของ USART_RTOR.",
                "counter เริ่มนับจากตำแหน่ง stop bit ที่ต่างกันตาม STOP[1:0].",
                "เมื่อครบเวลา RTOF จะ set และเกิด interrupt ถ้า RTOIE = 1.",
            ]
        )
    )
    subsection(story, "40.5.17 Smartcard mode")
    story.extend(
        bullets(
            [
                "ใช้เมื่อ instance รองรับและเปิดด้วย SCEN ใน USART_CR3. ต้อง clear LINEN, HDSEL และ IREN. สามารถเปิด CLKEN เพื่อจ่าย clock ให้ smartcard.",
                "รองรับ ISO 7816-3 asynchronous protocol ทั้ง T=0 character mode และ T=1 block mode.",
                "ตั้ง USART เป็น 8 bits plus parity: M = 1 และ PCE = 1; ใช้ 1.5 stop bits สำหรับรับ/ส่งข้อมูล โดย STOP = 11. บางกรณีเลือก 0.5 stop bit สำหรับ reception ได้.",
                "TX ต้องเป็น open-drain เพราะเป็นเส้น bidirectional ร่วมกับ smartcard.",
                "NACK ใน smartcard เกิดเมื่อ parity error: receiver ดึง line low ช่วง stop/guard time ทำให้ฝั่งส่งเห็น framing error และ hardware สามารถ retry ตาม SCARCNT.",
                "Guard time ตั้งด้วย GT[7:0] ใน USART_GTPR. ใน smartcard mode TC อาจถูกหน่วงจน guard time ครบ ส่วน TCBGT ใช้รู้ว่าการส่ง frame จบก่อน guard time.",
                "T=1 block mode ใช้ RTO สำหรับ BWT/CWT โดยตั้ง RTO = BWT - 11 ก่อนรอ byte แรก และหลัง byte แรกตั้ง RTO = CWT - 11. BLEN ใน RTOR ใช้แจ้ง end-of-block ด้วย EOBF.",
                "Direct convention: LSB-first, logical 1 = line high, even parity; ใช้ MSBFIRST=0 และ DATAINV=0. Inverse convention: MSB-first, logical 1 = line low; ใช้ MSBFIRST=1 และ DATAINV=1.",
                "การรู้ convention จาก TS character ต้องทำด้วย software. วิธีหนึ่งคือเริ่ม direct แล้วถ้า parity error ค่อย reprogram เป็น inverse; อีกวิธีคือรับแบบ 9-bit/no-parity แล้วตรวจ pattern 0x103 หรือ 0x13B.",
            ]
        )
    )
    subsection(story, "40.5.18 IrDA SIR ENDEC")
    story.extend(
        bullets(
            [
                "เปิด IrDA ด้วย IREN ใน USART_CR3. ต้อง clear LINEN, STOP, CLKEN ใน CR2 และ SCEN, HDSEL ใน CR3.",
                "SIR ใช้ Return to Zero, Inverted modulation: logic 0 แทนด้วย infrared pulse. normal mode ส่ง pulse width 3/16 ของ bit period และรองรับถึง 115.2 kbaud.",
                "IrDA เป็น half-duplex. ขณะส่ง receiver side จะ ignore ข้อมูลรับ และขณะรับไม่ควรส่งเพราะข้อมูลอาจเสีย.",
                "receiver glitch filter ใช้ค่า PSC ใน USART_GTPR; pulse สั้นกว่า 1 PSC period ถูก reject, มากกว่า 2 periods ถูก accept. ENDEC ใช้ไม่ได้เมื่อ PSC = 0.",
                "low-power IrDA ใช้ pulse width = 3 เท่าของ low-power baud clock แทน 3/16 bit period. software ต้องจัดการ delay ระหว่าง transmit/receive ตามข้อกำหนด IrDA อย่างน้อย 10 ms.",
            ]
        )
    )
    subsection(story, "40.5.19 Continuous communication using DMA")
    minor(story, "Transmission using DMA")
    story.extend(
        bullets(
            [
                "เปิด DMAT ใน USART_CR3.",
                "ตั้ง DMA destination เป็น address ของ USART_TDR.",
                "ตั้ง DMA source เป็น memory buffer และตั้งจำนวน byte.",
                "ตั้ง priority และ interrupt half/full transfer ตามต้องการ.",
                "clear TC ด้วย TCCF ก่อนเริ่ม แล้ว enable DMA channel.",
                "เมื่อ DMA transfer complete แล้ว ต้องรอ TC = 1 เพื่อให้ frame สุดท้ายส่งออกจริงก่อนปิด USART หรือเข้า low-power.",
            ]
        )
    )
    minor(story, "Reception using DMA")
    story.extend(
        bullets(
            [
                "เปิด DMAR ใน USART_CR3.",
                "ตั้ง DMA source เป็น USART_RDR และ destination เป็น memory buffer.",
                "ตั้งจำนวน byte, priority และ interrupt ตามต้องการ แล้ว enable DMA channel.",
                "ถ้า FIFO mode เปิด DMA request ของ TX เกิดจาก TXFNF และของ RX เกิดจาก RXFNE.",
                "สำหรับ low-power reception ต้อง disable DMA ก่อนเข้า low-power และ enable ใหม่หลังออก ตามข้อควรระวังของ manual.",
            ]
        )
    )
    subsection(story, "40.5.20 RS232 hardware flow control และ RS485 Driver Enable")
    story.extend(
        bullets(
            [
                "RTS/CTS เปิดแยกกันได้ด้วย RTSE และ CTSE ใน USART_CR3.",
                "RTS flow control: เมื่อ receiver พร้อมรับ RTS อยู่ในสถานะ deasserted; เมื่อ receive register/FIFO เต็ม RTS asserted เพื่อบอกคู่สื่อสารให้หยุดหลัง frame ปัจจุบัน.",
                "CTS flow control: transmitter ตรวจ CTS ก่อนส่ง frame ถัดไป. ถ้า CTS asserted ระหว่างกำลังส่ง จะส่ง frame ปัจจุบันให้จบแล้วจึงหยุด.",
                "CTSIF set เมื่อ CTS input toggle และเกิด interrupt ได้เมื่อ CTSIE = 1. CTS ต้องเปลี่ยนสถานะก่อนจบ character อย่างน้อย 3 USART clock periods เพื่อพฤติกรรมถูกต้อง.",
                "RS-485 driver enable เปิดด้วย DEM. ตั้ง polarity ด้วย DEP. ตั้ง DE assertion/deassertion timing ด้วย DEAT[4:0] และ DEDT[4:0] หน่วยเป็น 1/8 หรือ 1/16 bit time ตาม oversampling.",
            ]
        )
    )
    subsection(story, "40.5.21 USART low-power management")
    story.extend(
        bullets(
            [
                "ตั้ง UESM เพื่อให้ USART ปลุก MCU จาก low-power mode ได้เมื่อ instance และ clock source รองรับ.",
                "ถ้า PCLK ถูก gate, USART ใช้ usart_wkup interrupt เพื่อร้องขอให้ระบบเปิด clock เมื่อมีงานที่ต้องเข้าถึง register/FIFO.",
                "FIFO disabled: wake source หลักคือ RXNE และต้อง set RXNEIE ก่อนเข้า low-power.",
                "FIFO enabled: wake source อาจเป็น RXFNE, RXFF, TXFE, RXFT หรือ TXFT ตาม interrupt ที่เปิดไว้.",
                "สามารถเลือก wake-up event เฉพาะด้วย WUS; เมื่อ event ตรวจพบ WUF จะ set และ interrupt เกิดถ้า WUFIE = 1.",
                "ก่อนเข้า low-power ต้องแน่ใจว่าไม่มี transfer ค้างอยู่. หลัง init receiver ควรเช็ก REACK. เมื่อใช้ DMA reception ต้อง disable DMA ก่อนเข้า low-power.",
                "ถ้าใช้ mute mode ร่วม low-power และ FIFO disabled ห้ามใช้ idle detection เป็น wake-up จาก mute ใน low-power; ถ้าใช้ address match ต้องเลือก wake source เป็น address match ด้วย.",
            ]
        )
    )


def add_low_power_interrupts(story: list):
    section(story, "40.6 USART in low-power modes")
    story.append(tbl(LOW_POWER_ROWS, [0.18, 0.82], tiny=True))
    story.append(p("สำหรับ Stop mode ต้องตรวจว่าตัว peripheral instance รองรับ wake-up จาก Stop mode ใด และถ้า instance ไม่ทำงานใน Stop mode ที่เลือก ต้อง disable ก่อนเข้า mode นั้น.", "note"))
    section(story, "40.7 USART interrupts")
    story.append(tbl(INTERRUPT_ROWS, [0.31, 0.12, 0.15, 0.29, 0.13], tiny=True))


def add_registers(story: list):
    section(
        story,
        "40.8 USART registers",
        [
            "Peripheral registers ต้อง access เป็น word 32 bits. ค่า Reserved ต้องคง reset value เสมอ. ตารางต่อไปนี้แปลความหมาย register/bit สำคัญจากหน้าที่ 1740-1774 และคงชื่อ bit field ตามต้นฉบับ.",
        ],
    )
    subsection(story, "40.8.1 USART_CR1 - FIFO mode enabled")
    story.append(tbl(CR1_ROWS, [0.11, 0.20, 0.69], tiny=True))
    subsection(story, "40.8.2 USART_CR1 - FIFO mode disabled / alternate")
    story.append(tbl(CR1_ALT_ROWS, [0.13, 0.18, 0.69], tiny=True))
    subsection(story, "40.8.3 USART_CR2")
    story.append(tbl(CR2_ROWS, [0.12, 0.20, 0.68], tiny=True))
    subsection(story, "40.8.4 USART_CR3")
    story.append(tbl(CR3_ROWS, [0.12, 0.20, 0.68], tiny=True))
    subsection(story, "40.8.5 ถึง 40.8.7 - BRR, GTPR, RTOR")
    story.append(tbl(BRR_ROWS, [0.18, 0.13, 0.20, 0.49], tiny=True))
    subsection(story, "40.8.8 USART_RQR")
    story.append(tbl(RQR_ROWS, [0.12, 0.20, 0.68], tiny=True))
    subsection(story, "40.8.9 USART_ISR - FIFO mode enabled")
    story.append(tbl(ISR_FIFO_ROWS, [0.12, 0.20, 0.68], tiny=True))
    subsection(story, "40.8.10 USART_ISR - FIFO mode disabled / alternate")
    story.append(tbl(ISR_ALT_ROWS, [0.12, 0.20, 0.68], tiny=True))
    subsection(story, "40.8.11 USART_ICR")
    story.append(tbl(ICR_ROWS, [0.12, 0.22, 0.66], tiny=True))
    subsection(story, "40.8.12 ถึง 40.8.14 - RDR, TDR, PRESC")
    story.append(tbl(DATA_ROWS, [0.20, 0.12, 0.22, 0.46], tiny=True))
    subsection(story, "40.8.15 USART register map")
    story.append(tbl(REGISTER_MAP_ROWS, [0.15, 0.25, 0.60], tiny=True))
    story.append(
        p(
            "Register boundary base address ของแต่ละ USART/UART instance ให้ดูส่วน Memory organization ของ RM0440. ตารางนี้แสดง offset ภายใน peripheral เท่านั้น.",
            "note",
        )
    )


def footer(canvas, doc):
    canvas.saveState()
    canvas.setFont(FONT, 7.5)
    canvas.setFillColor(colors.HexColor("#58656D"))
    text = "RM0440 STM32G4 USART/UART หน้า 1689-1774 - ฉบับภาษาไทย"
    canvas.drawString(doc.leftMargin, 10 * mm, text)
    canvas.drawRightString(A4[0] - doc.rightMargin, 10 * mm, f"หน้า {doc.page}")
    canvas.restoreState()


def build_pdf():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    doc = SimpleDocTemplate(
        str(OUT),
        pagesize=A4,
        rightMargin=17 * mm,
        leftMargin=17 * mm,
        topMargin=16 * mm,
        bottomMargin=17 * mm,
        title="RM0440 USART UART pages 1689-1774 Thai",
        author="OpenAI Codex",
        subject="Thai translated and structured reference for STM32G4 USART/UART",
    )
    story: list = []
    add_cover(story)
    add_intro(story)
    add_functional(story)
    add_low_power_interrupts(story)
    add_registers(story)
    doc.build(story, onFirstPage=footer, onLaterPages=footer)
    return OUT


if __name__ == "__main__":
    output = build_pdf()
    print(output)
