#!/usr/bin/env python3
"""Deterministic, standard-library-only OOXML presentation generator."""

from __future__ import annotations

import argparse
import hashlib
import posixpath
import sys
import zipfile
from pathlib import Path
from xml.etree import ElementTree as ET
from xml.sax.saxutils import escape


OUT_NAME = "verificacion_software_ads1263_milestone_4.pptx"
FIXED_ZIP_TIME = (2026, 10, 15, 16, 12, 2)
W, H = 40 / 3, 7.5
EMU = 914400

BG = "F4F1EA"
NAVY = "102A43"
INK = "243B53"
MUTED = "627D98"
TEAL = "008C8C"
TEAL_LIGHT = "D9EFEC"
ORANGE = "E76F51"
ORANGE_LIGHT = "F8DED5"
BLUE = "2F6B9A"
BLUE_LIGHT = "DCE8F2"
GREEN = "2E7D62"
GREEN_LIGHT = "DDEDE5"
WHITE = "FFFFFF"
LINE = "CAD5DF"
GRAY_LIGHT = "E8EDF2"


def emu(value: float) -> int:
    return int(round(value * EMU))


def xml_text(value: object) -> str:
    return escape(str(value), {'"': "&quot;"})


def color_fill(color: str) -> str:
    return f'<a:solidFill><a:srgbClr val="{color}"/></a:solidFill>'


def line_xml(color: str, width: int = 12700, dash: str | None = None) -> str:
    dash_xml = f'<a:prstDash val="{dash}"/>' if dash else ""
    return f'<a:ln w="{width}">{color_fill(color)}{dash_xml}</a:ln>'


def paragraph(text: str, size: float = 18, color: str = INK, bold: bool = False,
              align: str = "l", font: str = "Arial", before: int = 0,
              after: int = 0) -> str:
    b = ' b="1"' if bold else ""
    return (
        f'<a:p><a:pPr algn="{align}" marL="0" indent="0">'
        f'<a:spcBef><a:spcPts val="{before}"/></a:spcBef>'
        f'<a:spcAft><a:spcPts val="{after}"/></a:spcAft></a:pPr>'
        f'<a:r><a:rPr lang="es-AR" sz="{int(size * 100)}"{b} dirty="0">'
        f'<a:latin typeface="{font}"/><a:ea typeface="{font}"/><a:cs typeface="{font}"/>'
        f'{color_fill(color)}</a:rPr><a:t>{xml_text(text)}</a:t></a:r>'
        f'<a:endParaRPr lang="es-AR" sz="{int(size * 100)}"/></a:p>'
    )


class Slide:
    def __init__(self) -> None:
        self.shapes: list[str] = []
        self.next_id = 2

    def rect(self, x: float, y: float, w: float, h: float, fill: str,
             line: str | None = None, radius: bool = False, name: str = "Rect") -> None:
        sid = self.next_id
        self.next_id += 1
        geom = "roundRect" if radius else "rect"
        ln = line_xml(line) if line else '<a:ln><a:noFill/></a:ln>'
        self.shapes.append(
            f'<p:sp><p:nvSpPr><p:cNvPr id="{sid}" name="{name} {sid}"/>'
            f'<p:cNvSpPr/><p:nvPr/></p:nvSpPr><p:spPr>'
            f'<a:xfrm><a:off x="{emu(x)}" y="{emu(y)}"/>'
            f'<a:ext cx="{emu(w)}" cy="{emu(h)}"/></a:xfrm>'
            f'<a:prstGeom prst="{geom}"><a:avLst/></a:prstGeom>{color_fill(fill)}{ln}'
            f'</p:spPr></p:sp>'
        )

    def line(self, x1: float, y1: float, x2: float, y2: float, color: str = LINE,
             width: int = 18000, arrow: bool = False, dash: str | None = None) -> None:
        sid = self.next_id
        self.next_id += 1
        tail = '<a:tailEnd type="none"/>'
        head = '<a:headEnd type="none"/>'
        if arrow:
            tail = '<a:tailEnd type="triangle" w="sm" len="sm"/>'
        dash_xml = f'<a:prstDash val="{dash}"/>' if dash else ""
        self.shapes.append(
            f'<p:cxnSp><p:nvCxnSpPr><p:cNvPr id="{sid}" name="Connector {sid}"/>'
            f'<p:cNvCxnSpPr/><p:nvPr/></p:nvCxnSpPr><p:spPr>'
            f'<a:xfrm><a:off x="{emu(x1)}" y="{emu(y1)}"/>'
            f'<a:ext cx="{emu(x2-x1)}" cy="{emu(y2-y1)}"/></a:xfrm>'
            f'<a:prstGeom prst="line"><a:avLst/></a:prstGeom>'
            f'<a:ln w="{width}">{color_fill(color)}{dash_xml}{head}{tail}</a:ln>'
            f'</p:spPr></p:cxnSp>'
        )

    def text(self, x: float, y: float, w: float, h: float, paragraphs: list[str],
             fill: str | None = None, line: str | None = None, radius: bool = False,
             margin: float = 0.08, valign: str = "t", name: str = "Text") -> None:
        sid = self.next_id
        self.next_id += 1
        geom = "roundRect" if radius else "rect"
        fill_xml = color_fill(fill) if fill else '<a:noFill/>'
        ln = line_xml(line) if line else '<a:ln><a:noFill/></a:ln>'
        margin_emu = emu(margin)
        self.shapes.append(
            f'<p:sp><p:nvSpPr><p:cNvPr id="{sid}" name="{name} {sid}"/>'
            f'<p:cNvSpPr txBox="1"/><p:nvPr/></p:nvSpPr><p:spPr>'
            f'<a:xfrm><a:off x="{emu(x)}" y="{emu(y)}"/>'
            f'<a:ext cx="{emu(w)}" cy="{emu(h)}"/></a:xfrm>'
            f'<a:prstGeom prst="{geom}"><a:avLst/></a:prstGeom>{fill_xml}{ln}</p:spPr>'
            f'<p:txBody><a:bodyPr wrap="square" lIns="{margin_emu}" rIns="{margin_emu}" '
            f'tIns="{margin_emu}" bIns="{margin_emu}" anchor="{valign}"/>'
            f'<a:lstStyle/>{"".join(paragraphs)}</p:txBody></p:sp>'
        )

    def circle(self, x: float, y: float, d: float, fill: str, text: str,
               size: float = 18, color: str = WHITE) -> None:
        sid = self.next_id
        self.next_id += 1
        self.shapes.append(
            f'<p:sp><p:nvSpPr><p:cNvPr id="{sid}" name="Circle {sid}"/>'
            f'<p:cNvSpPr/><p:nvPr/></p:nvSpPr><p:spPr>'
            f'<a:xfrm><a:off x="{emu(x)}" y="{emu(y)}"/>'
            f'<a:ext cx="{emu(d)}" cy="{emu(d)}"/></a:xfrm>'
            f'<a:prstGeom prst="ellipse"><a:avLst/></a:prstGeom>{color_fill(fill)}'
            f'<a:ln><a:noFill/></a:ln></p:spPr><p:txBody>'
            f'<a:bodyPr anchor="ctr"/><a:lstStyle/>'
            f'{paragraph(text, size, color, True, "ctr")}</p:txBody></p:sp>'
        )

    def xml(self) -> str:
        group = (
            '<p:nvGrpSpPr><p:cNvPr id="1" name=""/><p:cNvGrpSpPr/><p:nvPr/></p:nvGrpSpPr>'
            '<p:grpSpPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="0" cy="0"/>'
            '<a:chOff x="0" y="0"/><a:chExt cx="0" cy="0"/></a:xfrm></p:grpSpPr>'
        )
        return (
            '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
            '<p:sld xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" '
            'xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" '
            'xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">'
            f'<p:cSld><p:bg><p:bgPr>{color_fill(BG)}<a:effectLst/></p:bgPr></p:bg>'
            f'<p:spTree>{group}{"".join(self.shapes)}</p:spTree></p:cSld>'
            '<p:clrMapOvr><a:masterClrMapping/></p:clrMapOvr></p:sld>'
        )


def add_header(slide: Slide, number: int, title: str, kicker: str, backup: bool = False) -> None:
    label = "RESPALDO" if backup else f"{number:02d} / 09"
    slide.text(0.55, 0.28, 2.0, 0.28, [paragraph(kicker.upper(), 10, TEAL, True)], margin=0)
    slide.text(0.55, 0.62, 11.4, 0.56, [paragraph(title, 27, NAVY, True)], margin=0)
    slide.text(11.82, 0.30, 0.95, 0.28, [paragraph(label, 9, MUTED, True, "r")], margin=0)
    slide.line(0.55, 1.25, 12.78, 1.25, LINE, 9000)


def add_footer(slide: Slide, evidence: str, page: int) -> None:
    slide.line(0.55, 6.96, 12.78, 6.96, LINE, 7000)
    slide.text(0.55, 7.02, 11.7, 0.25, [paragraph(f"Evidencia: {evidence}", 7.5, MUTED)], margin=0)
    slide.text(12.35, 7.02, 0.42, 0.25, [paragraph(str(page), 8, MUTED, True, "r")], margin=0)


def metric(slide: Slide, x: float, y: float, w: float, value: str, label: str,
           fill: str = WHITE, accent: str = TEAL) -> None:
    slide.rect(x, y, w, 1.2, fill, LINE, True)
    slide.rect(x, y, 0.08, 1.2, accent, None, True)
    slide.text(x + 0.22, y + 0.12, w - 0.34, 0.55, [paragraph(value, 27, NAVY, True)], margin=0)
    slide.text(x + 0.22, y + 0.72, w - 0.34, 0.32, [paragraph(label, 11, MUTED, True)], margin=0)


def evidence_note(lines: list[str], paths: list[str]) -> str:
    return "\n".join(lines + ["", "Evidencia:"] + [f"- {p}" for p in paths])


def build_slides() -> tuple[list[Slide], list[str], list[str]]:
    slides: list[Slide] = []
    titles: list[str] = []
    notes: list[str] = []

    # 1 - Cover
    s = Slide()
    s.rect(0, 0, 13.333, 7.5, NAVY)
    s.rect(0.55, 0.55, 0.12, 5.9, TEAL)
    s.text(0.92, 0.70, 2.7, 0.35, [paragraph("TSSE · MILESTONE 4", 12, "8FD6CF", True)], margin=0)
    s.text(0.92, 1.35, 10.8, 1.55, [paragraph("Verificación de software", 33, WHITE, True), paragraph("para adquisición ADS1263", 33, WHITE, True)], margin=0)
    s.text(0.95, 3.20, 9.6, 0.8, [paragraph("Estrategia host reproducible · CFT · unitarias · integración · cobertura · estático", 17, "D9E2EC")], margin=0)
    s.text(0.95, 4.38, 3.0, 0.88, [paragraph("72/72", 23, WHITE, True, "ctr"), paragraph("unitarias no-CFT", 10, "B9CBDC", True, "ctr"), paragraph("66 ADS1263 + 6 robustez acquisition", 8, "B9CBDC", False, "ctr")], fill="183B57", radius=True, valign="ctr")
    s.text(4.10, 4.38, 2.40, 0.88, [paragraph("24/24", 23, WHITE, True, "ctr"), paragraph("casos CFT PASS", 10, "B9CBDC", True, "ctr")], fill="183B57", radius=True, valign="ctr")
    s.text(6.65, 4.38, 2.70, 0.88, [paragraph("71/71 × 2", 23, WHITE, True, "ctr"), paragraph("integración PASS", 10, "B9CBDC", True, "ctr")], fill="183B57", radius=True, valign="ctr")
    s.text(9.50, 4.38, 2.30, 0.88, [paragraph("1", 23, "FFC6B5", True, "ctr"), paragraph("defecto confirmado", 10, "B9CBDC", True, "ctr")], fill="183B57", radius=True, valign="ctr")
    s.text(0.95, 6.30, 9.5, 0.35, [paragraph("Alcance: host-only · sin cambios de producción · sin hardware", 12, "B9CBDC", True)], margin=0)
    s.text(11.7, 6.30, 1.0, 0.35, [paragraph("01 / 09", 10, "8FD6CF", True, "r")], margin=0)
    slides.append(s); titles.append("Verificación de software para adquisición ADS1263")
    notes.append(evidence_note([
        "Objetivo: presentar qué se verificó, con qué límites y qué resultados son defendibles.",
        "72 unitarias no-CFT = 66 ADS1263 + 6 acquisition robustness; los 24 casos CFT se informan aparte.",
        "No mezclar esos grupos con las 71 integraciones por corrida.",
        "Anticipar desde el inicio: todo el resultado actual es host-only y DEF-001 sigue abierto.",
    ], [
        "docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/",
        "docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json",
        "docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md",
    ]))

    # 2 - System and scope
    s = Slide(); add_header(s, 2, "Sistema, baseline y frontera de la evidencia", "Qué se verificó")
    stages = [("CLI", "parámetros"), ("acquisition", "orquestación"), ("ADS1263", "protocolo"), ("HAL Linux", "SPI / GPIO"), ("hardware", "placa + ADC")]
    x = 0.65
    for i, (a, b) in enumerate(stages):
        fill = WHITE if i < 4 else GRAY_LIGHT
        border = TEAL if i < 3 else LINE
        s.text(x, 1.72, 2.15, 0.95, [paragraph(a, 17, NAVY, True, "ctr"), paragraph(b, 10, MUTED, False, "ctr")], fill=fill, line=border, radius=True, valign="ctr")
        if i < len(stages) - 1:
            s.line(x + 2.15, 2.19, x + 2.38, 2.19, TEAL if i < 3 else MUTED, 16000, True)
        x += 2.43
    s.rect(0.65, 3.05, 8.0, 0.08, TEAL)
    s.text(0.65, 3.22, 8.0, 0.42, [paragraph("FRONTERA ACTUAL: software host, SUT real con límites controlados", 13, TEAL, True)], margin=0)
    s.text(0.65, 3.78, 5.65, 1.55, [paragraph("Incluye", 15, NAVY, True), paragraph("• módulos C reales", 13), paragraph("• mocks / wrappers / fake HAL", 13), paragraph("• CLI, archivos y procesos host", 13), paragraph("• trazas y artefactos reproducibles", 13)], fill=WHITE, line=LINE, radius=True)
    s.text(6.52, 3.78, 5.65, 1.55, [paragraph("No incluye", 15, NAVY, True), paragraph("• SPI/GPIO físicos", 13), paragraph("• exactitud eléctrica del ADC", 13), paragraph("• frecuencia o tiempo real de target", 13), paragraph("• rama libgpiod no disponible", 13)], fill=WHITE, line=LINE, radius=True)
    s.text(0.65, 5.60, 11.52, 0.74, [paragraph("VALIDACIÓN EN PLACA: antecedente histórico informado; sin raw log reproducible en este repositorio. No es resultado TSSE actual.", 13, ORANGE, True, "ctr")], fill=ORANGE_LIGHT, line=ORANGE, radius=True, valign="ctr")
    s.text(0.65, 6.46, 11.52, 0.25, [paragraph("Baseline CFT de producción: c59b24017f5d · análisis estático M4 HEAD: 54a91ed4b684", 9.5, MUTED, True, "ctr")], margin=0)
    add_footer(s, "docs/architecture.md · docs/tsse/discrepancias.md (DSP-016) · .../run_1/summary.json:580-584", 2)
    slides.append(s); titles.append("Sistema, baseline y frontera de la evidencia")
    notes.append(evidence_note([
        "Explicar el flujo desde CLI hasta hardware. La línea turquesa marca la frontera verificada.",
        "La producción real se compila donde corresponde, pero SPI/GPIO y hardware se sustituyen o excluyen.",
        "Decir literalmente que la validación de placa es histórica, sin logs raw adjuntos, y no una ejecución de este trabajo.",
        "Distinguir snapshots: el diseño CFT modela baseline c59b24017f5daebb4e37512d799a0d07f589bce5; el análisis estático M4 registra HEAD 54a91ed4b684b5ac6f4221c8743768fad5083270.",
    ], ["docs/architecture.md", "docs/tsse/cft_acquisition_run_samples.md", "docs/tsse/discrepancias.md#dsp-016", "docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md", "docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json"]))

    # 3 - Strategy
    s = Slide(); add_header(s, 3, "La calidad se construyó por capas, no por un único número", "Estrategia")
    cards = [
        ("01", "Diseño y trazabilidad", "requisitos → riesgos → casos", BLUE_LIGHT, BLUE),
        ("02", "Unitarias + CFT", "contratos y control de flujo", TEAL_LIGHT, TEAL),
        ("03", "Integración host", "CLI, CSV, procesos, fake HAL", GREEN_LIGHT, GREEN),
        ("04", "Cobertura", "líneas, funciones y ramas", BLUE_LIGHT, BLUE),
        ("05", "Análisis estático", "hallazgos + disposición", ORANGE_LIGHT, ORANGE),
    ]
    for i, (n, title, desc, fill, accent) in enumerate(cards):
        x = 0.65 + i * 2.48
        s.text(x, 1.68, 2.23, 2.0, [paragraph(n, 12, accent, True), paragraph(title, 16, NAVY, True), paragraph(desc, 11, MUTED)], fill=fill, line=accent, radius=True)
    s.text(0.65, 4.05, 3.65, 1.42, [paragraph("Oráculos observables", 16, NAVY, True), paragraph("retorno · orden · conteos · archivos · cleanup", 13, INK)], fill=WHITE, line=LINE, radius=True, valign="ctr")
    s.text(4.53, 4.05, 3.65, 1.42, [paragraph("Repetibilidad", 16, NAVY, True), paragraph("runs limpios · manifests · checksums · raw", 13, INK)], fill=WHITE, line=LINE, radius=True, valign="ctr")
    s.text(8.40, 4.05, 3.77, 1.42, [paragraph("Lenguaje de cierre", 16, NAVY, True), paragraph("PASS ≠ hardware · BLOCKED ≠ clean", 13, INK)], fill=WHITE, line=LINE, radius=True, valign="ctr")
    s.text(0.65, 5.78, 11.52, 0.55, [paragraph("Principio: cada afirmación cuantitativa conserva su alcance y su ruta de evidencia.", 15, TEAL, True, "ctr")], margin=0, valign="ctr")
    add_footer(s, "docs/tsse/master_test_plan.md · docs/tsse/matriz_trazabilidad.md · docs/tsse/catalogo_casos.md", 3)
    slides.append(s); titles.append("Estrategia de calidad por capas")
    notes.append(evidence_note([
        "No se usa cobertura como sinónimo de calidad; se triangulan diseño, ejecución, cobertura y análisis.",
        "Los oráculos verifican efectos observables y orden, no sólo el código de salida.",
        "La disciplina de estados evita presentar un bloqueo como éxito o un antecedente como ejecución actual.",
    ], ["docs/tsse/master_test_plan.md", "docs/tsse/matriz_trazabilidad.md", "docs/tsse/catalogo_casos.md"]))

    # 4 - CFT example
    s = Slide(); add_header(s, 4, "CFT: 24 paths factibles; ejemplo legible P21", "Control de flujo")
    s.text(0.65, 1.52, 3.0, 0.56, [paragraph("32 decisiones", 22, NAVY, True, "ctr"), paragraph("V(G)=33", 11, MUTED, True, "ctr")], fill=BLUE_LIGHT, line=BLUE, radius=True, valign="ctr")
    s.text(3.88, 1.52, 3.0, 0.56, [paragraph("24 paths", 22, NAVY, True, "ctr"), paragraph("selección factible", 11, MUTED, True, "ctr")], fill=TEAL_LIGHT, line=TEAL, radius=True, valign="ctr")
    s.text(7.10, 1.52, 5.07, 0.56, [paragraph("Frontera: acquisition, profundidad 1", 15, NAVY, True, "ctr"), paragraph("ADS1263/logger/archivo/tiempo como contratos", 10, MUTED, False, "ctr")], fill=WHITE, line=LINE, radius=True, valign="ctr")
    flow = [("1", "VALIDAR", "N=2 · rate=2 · ch=0"), ("2", "PREPARAR", "init → presence → configure"), ("3", "LEER #1", "S1 · output"), ("4", "ESPERAR", "1 sleep · back edge"), ("5", "LEER #2", "S2 · output → close")]
    x = 0.72
    for i, (n, title, desc) in enumerate(flow):
        s.circle(x, 3.05, 0.40, TEAL, n, 13)
        s.text(x - 0.18, 3.58, 2.08, 0.82, [paragraph(title, 13, NAVY, True, "ctr"), paragraph(desc, 9.5, MUTED, False, "ctr")], fill=WHITE, line=LINE, radius=True, valign="ctr")
        if i < 4:
            s.line(x + 0.42, 3.25, x + 2.36, 3.25, TEAL, 18000, True)
        x += 2.46
    s.text(0.72, 4.78, 5.55, 1.0, [paragraph("ORÁCULO", 11, TEAL, True), paragraph("return 0 · 2 reads · 2 outputs · 1 sleep · 1 close", 15, NAVY, True)], fill=TEAL_LIGHT, line=TEAL, radius=True, valign="ctr")
    s.text(6.50, 4.78, 5.67, 1.0, [paragraph("POR QUÉ ESTE PATH", 11, ORANGE, True), paragraph("demuestra la única vuelta finita y evita rate=1 / DEF-001", 14, NAVY, True)], fill=ORANGE_LIGHT, line=ORANGE, radius=True, valign="ctr")
    s.text(0.72, 6.02, 11.45, 0.42, [paragraph("No se afirma “all paths”: hay edges infeasibles y retries ambientales no acotados.", 12, MUTED, True, "ctr")], margin=0)
    add_footer(s, "docs/tsse/cft_acquisition_run_samples.md:107-160 · docs/tsse/cft_cases.md:61,92", 4)
    slides.append(s); titles.append("CFT: ejemplo P21")
    notes.append(evidence_note([
        "Presentar el CFT como un modelo de componente, no como un dibujo decorativo.",
        "P21 usa N=2 y rate=2: valida, prepara, produce dos muestras y recorre una vez el back edge.",
        "El oráculo controla retorno, llamadas, outputs, sleep y cleanup. Rate=1 con N>1 se excluye como path exitoso porque expone DEF-001.",
        "La selección de 24 paths cubre outcomes relevantes, pero no pretende todos los paths posibles.",
    ], ["docs/tsse/cft_acquisition_run_samples.md", "docs/tsse/cft_cases.md#paths-seleccionados", "docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json"]))

    # 5 - Unit/mock architecture
    s = Slide(); add_header(s, 5, "Arquitectura unitaria: producción real, entorno controlado", "Aislamiento")
    s.text(0.75, 1.62, 3.05, 3.85, [paragraph("DOBLES CONTROLADOS", 12, MUTED, True, "ctr"), paragraph("CMock ADS1263", 16, NAVY, True, "ctr"), paragraph("wrappers stdio / wait", 14, INK, False, "ctr"), paragraph("logger spy", 14, INK, False, "ctr"), paragraph("scripts de outcomes", 14, INK, False, "ctr")], fill=BLUE_LIGHT, line=BLUE, radius=True, valign="ctr")
    s.line(3.83, 3.52, 4.68, 3.52, TEAL, 23000, True)
    s.text(4.72, 1.62, 3.90, 3.85, [paragraph("SUT REAL", 12, TEAL, True, "ctr"), paragraph("ads1263.c", 23, NAVY, True, "ctr"), paragraph("+", 15, MUTED, True, "ctr"), paragraph("acquisition.c", 23, NAVY, True, "ctr"), paragraph("compilados en host", 13, MUTED, False, "ctr")], fill=WHITE, line=TEAL, radius=True, valign="ctr")
    s.line(8.65, 3.52, 9.50, 3.52, TEAL, 23000, True)
    s.text(9.54, 1.62, 2.85, 3.85, [paragraph("ORÁCULOS", 12, MUTED, True, "ctr"), paragraph("status", 15, NAVY, True, "ctr"), paragraph("orden y conteos", 15, NAVY, True, "ctr"), paragraph("muestras / CSV", 15, NAVY, True, "ctr"), paragraph("cleanup", 15, NAVY, True, "ctr")], fill=GREEN_LIGHT, line=GREEN, radius=True, valign="ctr")
    s.text(0.75, 5.80, 5.55, 0.64, [paragraph("72 no-CFT", 23, NAVY, True, "ctr"), paragraph("66 ADS1263 + 6 robustez acquisition", 10, MUTED, True, "ctr")], fill=WHITE, line=LINE, radius=True, valign="ctr")
    s.text(6.52, 5.80, 5.87, 0.64, [paragraph("24 CFT", 23, NAVY, True, "ctr"), paragraph("P01–P24, todos PASS", 10, MUTED, True, "ctr")], fill=TEAL_LIGHT, line=TEAL, radius=True, valign="ctr")
    add_footer(s, ".../ads1263_tests_report.json · .../acquisition_tests_report.json · .../04_acquisition_cft_clean_test_all_after_fix.typescript", 5)
    slides.append(s); titles.append("Arquitectura unitaria y mocks")
    notes.append(evidence_note([
        "La unidad bajo prueba no es una reimplementación: se compilan los C de producción.",
        "Los dobles controlan dependencias y permiten verificar secuencias, errores, salidas y cleanup sin hardware.",
        "Descomponer el total: 66 pruebas del core ADS1263; 6 de robustez acquisition; 24 paths CFT.",
    ], ["docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/ads1263_tests_report.json", "docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json", "test/ceedling/project_acquisition_robustness.yml"]))

    # 6 - Results
    s = Slide(); add_header(s, 6, "Resultado actual: ejecución limpia y repetible en host", "Resultados")
    metric(s, 0.65, 1.55, 3.67, "72 / 72", "UNITARIAS NO-CFT · PASS", WHITE, BLUE)
    metric(s, 4.55, 1.55, 3.67, "24 / 24", "PATHS CFT · PASS", WHITE, TEAL)
    metric(s, 8.45, 1.55, 3.72, "71 / 71", "INTEGRACIÓN · RUN 1 PASS", WHITE, GREEN)
    metric(s, 8.45, 3.02, 3.72, "71 / 71", "INTEGRACIÓN · RUN 2 PASS", GREEN_LIGHT, GREEN)
    s.text(0.65, 3.02, 7.57, 1.20, [paragraph("Las dos corridas formales declaran y ejecutan exactamente 71 escenarios", 20, NAVY, True, "ctr"), paragraph("0 failed · 0 blocked · declaration_matches_execution=true", 12, MUTED, True, "ctr")], fill=TEAL_LIGHT, line=TEAL, radius=True, valign="ctr")
    s.text(0.65, 4.62, 5.60, 1.22, [paragraph("Qué cubre integración", 14, NAVY, True), paragraph("CLI · CSV · procesos · signals · fake HAL · errores", 13, INK)], fill=WHITE, line=LINE, radius=True, valign="ctr")
    s.text(6.47, 4.62, 5.70, 1.22, [paragraph("Qué no prueba", 14, NAVY, True), paragraph("bus físico · libgpiod real · ADC · timing target", 13, INK)], fill=ORANGE_LIGHT, line=ORANGE, radius=True, valign="ctr")
    s.text(0.65, 6.08, 11.52, 0.36, [paragraph("Sin cambios de producción y sin acceso a hardware durante estas ejecuciones.", 12, MUTED, True, "ctr")], margin=0)
    add_footer(s, ".../04_unitarias/milestone_4_20261015T161203Z/*.json · .../05_integracion_host/.../run_{1,2}/summary.json", 6)
    slides.append(s); titles.append("Resultados actuales")
    notes.append(evidence_note([
        "Dar los cuatro números sin sumarlos en un total ambiguo: 72, 24, 71 y 71.",
        "La repetición de integración confirma declaración versus ejecución y conserva resultados por caso.",
        "Aclarar que PASS significa conformidad dentro del contrato host; no valida hardware.",
    ], ["docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/ads1263_tests_report.json", "docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json", "docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json", "docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_2/summary.json"]))

    # 7 - Coverage
    s = Slide(); add_header(s, 7, "Cobertura por módulo y trazabilidad hasta la evidencia", "Medición")
    s.text(0.65, 1.52, 5.60, 0.38, [paragraph("ads1263.c · estado actual", 16, NAVY, True)], margin=0)
    cov = [("Líneas", 151, 222, BLUE), ("Funciones", 13, 16, TEAL), ("Ramas", 90, 134, GREEN)]
    y = 2.02
    for label, num, den, col in cov:
        pct = num / den
        s.text(0.65, y, 1.2, 0.32, [paragraph(label, 12, INK, True)], margin=0)
        s.rect(1.88, y + 0.02, 3.08, 0.24, GRAY_LIGHT, None, True)
        s.rect(1.88, y + 0.02, 3.08 * pct, 0.24, col, None, True)
        s.text(5.08, y - 0.02, 1.14, 0.33, [paragraph(f"{num}/{den}", 12, NAVY, True, "r")], margin=0)
        y += 0.67
    s.text(6.55, 1.52, 5.62, 0.38, [paragraph("acquisition.c · avance", 16, NAVY, True)], margin=0)
    rows = [("Líneas", "54/168", "118/168", 54/168, 118/168), ("Funciones", "6/13", "9/13", 6/13, 9/13), ("Ramas", "26/76", "53/76", 26/76, 53/76)]
    y = 2.02
    for label, prior, now, p1, p2 in rows:
        s.text(6.55, y, 1.2, 0.32, [paragraph(label, 12, INK, True)], margin=0)
        s.rect(7.78, y + 0.02, 3.05, 0.24, GRAY_LIGHT, None, True)
        s.rect(7.78, y + 0.02, 3.05 * p2, 0.24, TEAL, None, True)
        s.line(7.78 + 3.05 * p1, y - 0.03, 7.78 + 3.05 * p1, y + 0.32, ORANGE, 18000)
        s.text(10.97, y - 0.02, 1.18, 0.33, [paragraph(now, 12, NAVY, True, "r")], margin=0)
        y += 0.67
    s.text(6.55, 4.12, 5.60, 0.32, [paragraph("marca naranja = medición previa", 10, ORANGE, True, "r")], margin=0)
    s.text(0.65, 4.78, 3.65, 1.10, [paragraph("REQUISITO", 11, MUTED, True, "ctr"), paragraph("REQ / TST / ROB", 17, NAVY, True, "ctr")], fill=WHITE, line=LINE, radius=True, valign="ctr")
    s.line(4.30, 5.33, 4.52, 5.33, TEAL, 18000, True)
    s.text(4.55, 4.78, 3.65, 1.10, [paragraph("CASO", 11, MUTED, True, "ctr"), paragraph("ID + oráculo", 17, NAVY, True, "ctr")], fill=WHITE, line=LINE, radius=True, valign="ctr")
    s.line(8.20, 5.33, 8.42, 5.33, TEAL, 18000, True)
    s.text(8.45, 4.78, 3.72, 1.10, [paragraph("RESULTADO", 11, MUTED, True, "ctr"), paragraph("raw + resumen", 17, NAVY, True, "ctr")], fill=TEAL_LIGHT, line=TEAL, radius=True, valign="ctr")
    s.text(0.65, 6.18, 11.52, 0.28, [paragraph("Cobertura guía preguntas; no existe un umbral inventado ni una afirmación de suficiencia por porcentaje.", 11, MUTED, True, "ctr")], margin=0)
    add_footer(s, ".../06_cobertura/milestone_4_.../{ads1263,acquisition}/ · docs/tsse/{matriz_trazabilidad,catalogo_casos}.md", 7)
    slides.append(s); titles.append("Cobertura y trazabilidad")
    notes.append(evidence_note([
        "Leer cobertura por módulo y métrica; no combinar denominadores.",
        "ADS1263 queda en 151/222 líneas, 13/16 funciones y 90/134 ramas.",
        "Acquisition sube desde 54/168, 6/13 y 26/76 a 118/168, 9/13 y 53/76.",
        "Cerrar la lámina mostrando la cadena requisito → caso → evidencia, sin umbral artificial.",
    ], ["docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/ads1263/GcovCoverage.json", "docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/acquisition/GcovCoverage.json", "docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/README.md", "docs/tsse/matriz_trazabilidad.md", "docs/tsse/catalogo_casos.md"]))

    # 8 - Findings
    s = Slide(); add_header(s, 8, "Hallazgos: distinguir diagnóstico, bloqueo y defecto", "Riesgo")
    s.text(0.65, 1.55, 3.65, 3.75, [paragraph("CPPCHECK", 12, MUTED, True), paragraph("41", 35, NAVY, True), paragraph("hallazgos style", 14, INK, True), paragraph("0 errores / warnings / performance / portability", 12, MUTED), paragraph("0 defectos nuevos confirmados", 14, GREEN, True)], fill=WHITE, line=LINE, radius=True)
    s.text(4.53, 1.55, 3.65, 3.75, [paragraph("CONFIGURACIÓN", 12, MUTED, True), paragraph("BLOCKED", 27, ORANGE, True), paragraph("rama libgpiod", 14, INK, True), paragraph("pkg-config ausente", 12, MUTED), paragraph("gpiod.h ausente", 12, MUTED), paragraph("no se afirma clean", 14, ORANGE, True)], fill=ORANGE_LIGHT, line=ORANGE, radius=True)
    s.text(8.40, 1.55, 3.77, 3.75, [paragraph("DEF-001", 12, MUTED, True), paragraph("1", 35, ORANGE, True), paragraph("único defecto confirmado", 14, INK, True), paragraph("rate=1 → tv_nsec=1e9", 12, MUTED), paragraph("EINVAL + retry sin progreso", 12, MUTED), paragraph("abierto · sin corrección", 14, ORANGE, True)], fill=ORANGE_LIGHT, line=ORANGE, radius=True)
    s.text(0.65, 5.58, 5.60, 0.70, [paragraph("ROB-001", 12, BLUE, True), paragraph("efecto intraobjeto observado · defecto sospechado", 12, NAVY, True)], fill=BLUE_LIGHT, line=BLUE, radius=True, valign="ctr")
    s.text(6.47, 5.58, 5.70, 0.70, [paragraph("ROB-003", 12, TEAL, True), paragraph("comportamiento reproducido · ambigüedad contractual", 12, NAVY, True)], fill=TEAL_LIGHT, line=TEAL, radius=True, valign="ctr")
    add_footer(s, ".../02_analisis_estatico/milestone_4_.../{README,findings}.md · .../DEF-001-DSP017-rate-1-livelock.md", 8)
    slides.append(s); titles.append("Hallazgos estáticos y DEF-001")
    notes.append(evidence_note([
        "Separar explícitamente tres categorías.",
        "Cppcheck produjo 41 diagnósticos style: 37 unusedStructMember, 1 constParameterPointer de contexto/tool y 3 staticFunction de interfaz pública aceptados.",
        "Los 32 missingIncludeSystem son notices separados; el total no equivale a ejecución limpia ni a ausencia total de defectos.",
        "La variante libgpiod está BLOCKED por entorno y no se ejecutó.",
        "DEF-001 es el único defecto confirmado: rate=1 construye un timespec inválido y el proceso no progresa para N>=2.",
        "ROB-001 sigue sospechado por falta de contrato aprobado; ROB-003 sigue como ambigüedad contractual.",
    ], ["docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md", "docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/findings.md", "docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md", "docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md"]))

    # 9 - Conclusions
    s = Slide(); add_header(s, 9, "Conclusión: evidencia fuerte para software host, no para hardware", "Cierre")
    s.text(0.65, 1.52, 5.62, 3.70, [paragraph("PODEMOS AFIRMAR", 12, GREEN, True), paragraph("✓ contratos unitarios repetibles", 17, NAVY, True), paragraph("✓ 24 paths CFT ejecutados", 17, NAVY, True), paragraph("✓ integración host en dos runs", 17, NAVY, True), paragraph("✓ cobertura por módulo trazable", 17, NAVY, True), paragraph("✓ findings con disposición", 17, NAVY, True)], fill=GREEN_LIGHT, line=GREEN, radius=True)
    s.text(6.52, 1.52, 5.65, 3.70, [paragraph("NO PODEMOS AFIRMAR", 12, ORANGE, True), paragraph("× comportamiento eléctrico", 17, NAVY, True), paragraph("× SPI / GPIO físicos", 17, NAVY, True), paragraph("× exactitud del ADS1263", 17, NAVY, True), paragraph("× tiempo real de target", 17, NAVY, True), paragraph("× rama libgpiod analizada", 17, NAVY, True)], fill=ORANGE_LIGHT, line=ORANGE, radius=True)
    s.text(0.65, 5.52, 3.65, 0.80, [paragraph("1", 24, ORANGE, True, "ctr"), paragraph("resolver DEF-001 con gate", 11, NAVY, True, "ctr")], fill=WHITE, line=ORANGE, radius=True, valign="ctr")
    s.text(4.53, 5.52, 3.65, 0.80, [paragraph("2", 24, TEAL, True, "ctr"), paragraph("habilitar libgpiod y repetir", 11, NAVY, True, "ctr")], fill=WHITE, line=TEAL, radius=True, valign="ctr")
    s.text(8.40, 5.52, 3.77, 0.80, [paragraph("3", 24, BLUE, True, "ctr"), paragraph("validar en placa con raw log", 11, NAVY, True, "ctr")], fill=WHITE, line=BLUE, radius=True, valign="ctr")
    add_footer(s, ".../run_1/summary.json:580-584 · .../02_analisis_estatico/.../README.md:139-144 · DEF-001 report", 9)
    slides.append(s); titles.append("Conclusiones y limitaciones")
    notes.append(evidence_note([
        "Cerrar con una afirmación calibrada: evidencia fuerte de contratos software host, no certificación del sistema físico.",
        "Los siguientes pasos naturales son: gate de corrección para DEF-001, entorno con libgpiod y una campaña de placa con logs raw.",
        "No prometer esas actividades como realizadas.",
    ], ["docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json", "docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md", "docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md", "docs/tsse/discrepancias.md#dsp-016"]))

    # 10 - Backup CFT groups
    s = Slide(); add_header(s, 10, "Todos los grupos de paths CFT", "Detalle del diseño", True)
    groups = [
        ("P01–P08", "Validación", "nulls · límites · 3 errores", BLUE_LIGHT, BLUE),
        ("P09–P11", "Inicialización", "SPI · GPIO · genérico", ORANGE_LIGHT, ORANGE),
        ("P12–P15", "Presencia", "ZERO · FF · mismatch · IO", TEAL_LIGHT, TEAL),
        ("P16", "Configuración", "fallo configure", GRAY_LIGHT, MUTED),
        ("P17–P19", "CSV / éxito", "sin CSV · open fail · CSV OK", GREEN_LIGHT, GREEN),
        ("P20 / P22", "Lectura", "fallo temprano · tardío", ORANGE_LIGHT, ORANGE),
        ("P21 / P23", "Loop / sleep", "back edge · EINTR retry", TEAL_LIGHT, TEAL),
        ("P24", "Límite alto", "N=1000 · fallo temprano", BLUE_LIGHT, BLUE),
    ]
    for i, (ids, title, desc, fill, accent) in enumerate(groups):
        x = 0.65 + (i % 4) * 3.03
        y = 1.58 + (i // 4) * 2.12
        s.text(x, y, 2.78, 1.72, [paragraph(ids, 12, accent, True), paragraph(title, 17, NAVY, True), paragraph(desc, 11, MUTED)], fill=fill, line=accent, radius=True)
    s.text(0.65, 5.98, 11.52, 0.42, [paragraph("24/24 PASS · selección de outcomes factibles, no all-paths", 13, TEAL, True, "ctr")], margin=0)
    add_footer(s, "docs/tsse/cft_cases.md:114-129 · .../acquisition_tests_report.json", 10)
    slides.append(s); titles.append("Respaldo: grupos CFT")
    notes.append(evidence_note(["Usar sólo si preguntan cómo se distribuyen los 24 paths.", "Cada grupo corresponde a outcomes y edges explícitos del diseño CFT."], ["docs/tsse/cft_cases.md", "docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json"]))

    # 11 - Backup inventory
    s = Slide(); add_header(s, 11, "Inventario de pruebas ejecutadas", "Detalle cuantitativo", True)
    headers = ["Suite", "Casos", "Resultado", "Foco"]
    widths = [3.25, 1.4, 1.8, 5.05]
    x0, y0 = 0.65, 1.55
    x = x0
    for h, w in zip(headers, widths):
        s.text(x, y0, w, 0.48, [paragraph(h, 11, WHITE, True, "ctr")], fill=NAVY, line=NAVY, valign="ctr")
        x += w
    rows = [
        ("ADS1263 unit", "66", "66 PASS", "registros, presencia, canal, muestras, DRDY, robustez"),
        ("Acquisition no-CFT", "6", "6 PASS", "ROB-003 y DSP-017 unitario"),
        ("Acquisition CFT", "24", "24 PASS", "P01–P24, control de flujo depth-1"),
        ("Integración run 1", "71", "71 PASS", "CLI, CSV, strace, signal, fake HAL"),
        ("Integración run 2", "71", "71 PASS", "repetición formal independiente"),
        ("Probe DEF-001", "1", "FAIL esperado", "timeout 124 confirma no terminación"),
    ]
    y = 2.03
    for idx, row in enumerate(rows):
        x = x0; fill = WHITE if idx % 2 == 0 else GRAY_LIGHT
        for value, w in zip(row, widths):
            color = ORANGE if "FAIL" in value else INK
            s.text(x, y, w, 0.66, [paragraph(value, 11, color, "PASS" in value or "FAIL" in value, "ctr")], fill=fill, line=LINE, valign="ctr")
            x += w
        y += 0.66
    s.text(0.65, 6.16, 11.52, 0.32, [paragraph("No sumar repeticiones como casos únicos; el probe de conformidad no es fallo de infraestructura.", 11, MUTED, True, "ctr")], margin=0)
    add_footer(s, ".../04_unitarias/milestone_4_.../*.json · .../05_integracion_host/.../summary.json · milestone_3a_completion_report.md", 11)
    slides.append(s); titles.append("Respaldo: inventario de pruebas")
    notes.append(evidence_note(["Distinguir casos únicos, corridas repetidas y el probe del defecto.", "Los 72 no-CFT son 66 ADS1263 más 6 acquisition robustness."], ["docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/ads1263_tests_report.json", "docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json", "docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json", "docs/tsse/evidencias/milestone_3a_completion_report.md"]))

    # 12 - Raw excerpts
    s = Slide(); add_header(s, 12, "Extractos raw: resultados sin reinterpretación", "Evidencia primaria", True)
    s.text(0.65, 1.55, 5.65, 2.02, [paragraph("CEEDLING · CFT + robustez", 11, TEAL, True), paragraph("TESTED:  30", 14, NAVY, True, font="Courier New"), paragraph("PASSED:  30", 14, NAVY, True, font="Courier New"), paragraph("FAILED:   0", 14, NAVY, True, font="Courier New"), paragraph('COMMAND_EXIT_CODE="0"', 12, MUTED, False, font="Courier New")], fill="F8FAFC", line=LINE, radius=True)
    s.text(6.52, 1.55, 5.65, 2.02, [paragraph("INTEGRACIÓN · RUN 1", 11, GREEN, True), paragraph("SUMMARY: PASS scenarios=71", 13, NAVY, True, font="Courier New"), paragraph("passed=71 failed=0 blocked=0", 13, NAVY, True, font="Courier New"), paragraph("declaration_match=True", 13, NAVY, True, font="Courier New")], fill="F8FAFC", line=LINE, radius=True)
    s.text(0.65, 3.82, 5.65, 2.02, [paragraph("COBERTURA · ACQUISITION", 11, BLUE, True), paragraph("Lines:     118 / 168", 14, NAVY, True, font="Courier New"), paragraph("Functions:   9 / 13", 14, NAVY, True, font="Courier New"), paragraph("Branches:   53 / 76", 14, NAVY, True, font="Courier New")], fill="F8FAFC", line=LINE, radius=True)
    s.text(6.52, 3.82, 5.65, 2.02, [paragraph("DEF-001 · PROBE", 11, ORANGE, True), paragraph("samples=2 rate_hz=1", 14, NAVY, True, font="Courier New"), paragraph("tv_nsec=1000000000", 14, NAVY, True, font="Courier New"), paragraph("timeout → exit 124", 14, ORANGE, True, font="Courier New")], fill=ORANGE_LIGHT, line=ORANGE, radius=True)
    s.text(0.65, 6.16, 11.52, 0.28, [paragraph("Extractos tipografiados; consultar los archivos raw enlazados para el contexto completo.", 10, MUTED, True, "ctr")], margin=0)
    add_footer(s, ".../04_acquisition_cft_clean_test_all_after_fix.typescript · .../02_run_1.typescript · coverage JSON · DEF-001 report", 12)
    slides.append(s); titles.append("Respaldo: extractos raw")
    notes.append(evidence_note(["Los extractos son deliberadamente breves; no sustituyen los raw.", "El bloque de cobertura proviene del JSON de gcovr; el probe DEF-001 está documentado con comando y timeout."], ["docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/04_acquisition_cft_clean_test_all_after_fix.typescript", "docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/02_run_1.typescript", "docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/acquisition/GcovCoverage.json", "docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/03_dsp017_probe_timeout.typescript"]))

    # 13 - Module coverage
    s = Slide(); add_header(s, 13, "Cobertura detallada por módulo", "Denominadores separados", True)
    cols = [("ads1263.c", [("Líneas", "151/222", "68.0%"), ("Funciones", "13/16", "81.2%"), ("Ramas", "90/134", "67.2%")], BLUE), ("acquisition.c", [("Líneas", "118/168", "70.2%"), ("Funciones", "9/13", "69.2%"), ("Ramas", "53/76", "69.7%")], TEAL)]
    for i, (name, rows2, accent) in enumerate(cols):
        x = 0.65 + i * 5.92
        s.text(x, 1.55, 5.60, 0.60, [paragraph(name, 20, WHITE, True, "ctr")], fill=accent, line=accent, radius=True, valign="ctr")
        y = 2.32
        for label, fraction, pct in rows2:
            s.text(x, y, 2.20, 0.77, [paragraph(label, 14, NAVY, True)], fill=WHITE, line=LINE, valign="ctr")
            s.text(x + 2.20, y, 1.85, 0.77, [paragraph(fraction, 16, NAVY, True, "ctr")], fill=WHITE, line=LINE, valign="ctr")
            s.text(x + 4.05, y, 1.55, 0.77, [paragraph(pct, 16, accent, True, "ctr")], fill=WHITE, line=LINE, valign="ctr")
            y += 0.77
    s.text(0.65, 5.10, 11.52, 0.82, [paragraph("Comparación acquisition", 12, ORANGE, True, "ctr"), paragraph("previo: 54/168 · 6/13 · 26/76  →  actual: 118/168 · 9/13 · 53/76", 16, NAVY, True, "ctr")], fill=ORANGE_LIGHT, line=ORANGE, radius=True, valign="ctr")
    s.text(0.65, 6.18, 11.52, 0.25, [paragraph("Sin umbral de aceptación inventado; los gaps quedan visibles para priorización.", 10, MUTED, True, "ctr")], margin=0)
    add_footer(s, ".../06_cobertura/milestone_4_.../{ads1263,acquisition}/GcovCoverage.json · milestone_3a .../README.md", 13)
    slides.append(s); titles.append("Respaldo: cobertura por módulo")
    notes.append(evidence_note(["No sumar líneas de módulos ni convertirlas en una cobertura global no definida.", "La mejora de acquisition refleja la incorporación de los paths CFT."], ["docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/ads1263/GcovCoverage.json", "docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/acquisition/GcovCoverage.json", "docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/README.md"]))

    # 14 - Static dispositions
    s = Slide(); add_header(s, 14, "Disposición de análisis estático", "41 findings style", True)
    data = [
        ("1", "constParameterPointer", "falso positivo por escritura kernel ioctl", "AR-SA-001"),
        ("37", "unusedStructMember", "contexto de headers / configuración", "AR-SA-002"),
        ("3", "staticFunction", "API pública aceptada", "AR-SA-003"),
        ("32", "missingIncludeSystem", "limitación visible, no finding source", "AR-SA-004"),
        ("2", "branch-depth notices", "cerrados por follow-up exhaustive", "AR-SA-005"),
        ("1", "libgpiod config", "BLOCKED por entorno", "BLK-SA-001"),
    ]
    headers = ["N", "Clase", "Disposición", "Vínculo"]
    widths = [0.75, 2.75, 6.10, 1.90]
    x0, y0 = 0.65, 1.52
    x = x0
    for h, w in zip(headers, widths):
        s.text(x, y0, w, 0.48, [paragraph(h, 10, WHITE, True, "ctr")], fill=NAVY, line=NAVY, valign="ctr"); x += w
    y = 2.00
    for idx, row in enumerate(data):
        x = x0; fill = WHITE if idx % 2 == 0 else GRAY_LIGHT
        for value, w in zip(row, widths):
            color = ORANGE if "BLOCKED" in value else INK
            s.text(x, y, w, 0.67, [paragraph(value, 10.5, color, value.startswith("AR-") or value.startswith("BLK"), "ctr")], fill=fill, line=LINE, valign="ctr"); x += w
        y += 0.67
    s.text(0.65, 6.15, 11.52, 0.30, [paragraph("41 source findings únicos · 0 defectos confirmados nuevos · configuración bloqueada no declarada clean", 11, TEAL, True, "ctr")], margin=0)
    add_footer(s, ".../02_analisis_estatico/milestone_4_.../findings.md · README.md:50-81", 14)
    slides.append(s); titles.append("Respaldo: disposiciones estáticas")
    notes.append(evidence_note(["Los 41 source findings son 37 unusedStructMember, 1 constParameterPointer de contexto/tool y 3 staticFunction de interfaz pública aceptados.", "Las 32 limitaciones missingIncludeSystem y los 2 notices de profundidad no pertenecen al total de 41 source findings.", "El follow-up exhaustive cerró los notices de profundidad sin borrar el run original."], ["docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/findings.md", "docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md"]))

    # 15 - Risks/defect
    s = Slide(); add_header(s, 15, "ROB-001, ROB-003 y DEF-001", "Clasificación y decisión", True)
    cards2 = [
        ("ROB-001", "SUSPECTED DEFECT", "cruce de mock_registers al byte adyacente", "ASan/UBSan silenciosos no prueban seguridad", BLUE_LIGHT, BLUE),
        ("ROB-003", "CONTRACT AMBIGUITY", "GPIO_READ / timeout pueden acompañar SPI OK", "falta contrato aprobado de fatalidad", TEAL_LIGHT, TEAL),
        ("DEF-001", "CONFIRMED DEFECT", "rate=1 crea tv_nsec=1000000000", "N≥2 no completa; gate humano pendiente", ORANGE_LIGHT, ORANGE),
    ]
    for i, (rid, state, observed, decision, fill, accent) in enumerate(cards2):
        x = 0.65 + i * 3.92
        s.text(x, 1.55, 3.65, 4.55, [paragraph(rid, 14, accent, True), paragraph(state, 16, NAVY, True), paragraph("OBSERVADO", 10, MUTED, True), paragraph(observed, 13, INK), paragraph("DISPOSICIÓN", 10, MUTED, True), paragraph(decision, 13, INK)], fill=fill, line=accent, radius=True)
    s.text(0.65, 6.28, 11.52, 0.24, [paragraph("Únicamente DEF-001 cumple el criterio de defecto confirmado.", 11, ORANGE, True, "ctr")], margin=0)
    add_footer(s, ".../07_defectos_regresion/milestone_3a_.../{README,DEF-001-DSP017-rate-1-livelock}.md", 15)
    slides.append(s); titles.append("Respaldo: ROB-001/003 y DEF-001")
    notes.append(evidence_note(["No promover ROB-001 ni ROB-003 a defectos confirmados.", "DEF-001 combina unit test de argumento, llamada POSIX y probe de proceso acotado."], ["docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md", "docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md"]))

    # 16 - Evidence map
    s = Slide(); add_header(s, 16, "Mapa de evidencia para auditoría y preguntas", "Dónde mirar", True)
    maps = [
        ("Diseño CFT", "docs/tsse/cft_acquisition_run_samples.md\ndocs/tsse/cft_cases.md", BLUE),
        ("Unitarias", "docs/tsse/evidencias/04_unitarias/\nmilestone_4_20261015T161203Z/", TEAL),
        ("Integración", "docs/tsse/evidencias/05_integracion_host/\nmilestone_4_20261015T161203Z/", GREEN),
        ("Cobertura", "docs/tsse/evidencias/06_cobertura/\nmilestone_4_20261015T161203Z/", BLUE),
        ("Defectos", "docs/tsse/evidencias/07_defectos_regresion/\nmilestone_3a_20261015T150616Z/", ORANGE),
        ("Estático", "docs/tsse/evidencias/02_analisis_estatico/\nmilestone_4_20261015T161203Z/", MUTED),
    ]
    for i, (title, path, accent) in enumerate(maps):
        x = 0.65 + (i % 2) * 5.92
        y = 1.52 + (i // 2) * 1.52
        ps = [paragraph(title, 14, accent, True)] + [paragraph(p, 10.5, NAVY, False, font="Courier New") for p in path.split("\n")]
        s.text(x, y, 5.60, 1.20, ps, fill=WHITE, line=accent, radius=True, valign="ctr")
    s.text(0.65, 6.18, 11.52, 0.30, [paragraph("Esta presentación y su verificación: docs/tsse/evidencias/09_presentacion/milestone_4_20261015T161203Z/", 9.5, MUTED, True, "ctr")], margin=0)
    add_footer(s, "README de evidencia de presentación · deck.sha256 · verification.stdout.txt", 16)
    slides.append(s); titles.append("Respaldo: mapa de evidencia")
    notes.append(evidence_note(["Usar como índice durante preguntas.", "Las rutas son relativas a la raíz del repositorio y preservan raw, resúmenes y disposiciones."], ["docs/tsse/evidencias/09_presentacion/milestone_4_20261015T161203Z/README.md"]))

    return slides, titles, notes


def content_types(slide_count: int) -> str:
    overrides = [
        ('/ppt/presentation.xml', 'application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml'),
        ('/ppt/slideMasters/slideMaster1.xml', 'application/vnd.openxmlformats-officedocument.presentationml.slideMaster+xml'),
        ('/ppt/slideLayouts/slideLayout1.xml', 'application/vnd.openxmlformats-officedocument.presentationml.slideLayout+xml'),
        ('/ppt/theme/theme1.xml', 'application/vnd.openxmlformats-officedocument.theme+xml'),
        ('/ppt/notesMasters/notesMaster1.xml', 'application/vnd.openxmlformats-officedocument.presentationml.notesMaster+xml'),
        ('/ppt/presProps.xml', 'application/vnd.openxmlformats-officedocument.presentationml.presProps+xml'),
        ('/ppt/viewProps.xml', 'application/vnd.openxmlformats-officedocument.presentationml.viewProps+xml'),
        ('/ppt/tableStyles.xml', 'application/vnd.openxmlformats-officedocument.presentationml.tableStyles+xml'),
        ('/docProps/core.xml', 'application/vnd.openxmlformats-package.core-properties+xml'),
        ('/docProps/app.xml', 'application/vnd.openxmlformats-officedocument.extended-properties+xml'),
    ]
    for i in range(1, slide_count + 1):
        overrides.append((f'/ppt/slides/slide{i}.xml', 'application/vnd.openxmlformats-officedocument.presentationml.slide+xml'))
        overrides.append((f'/ppt/notesSlides/notesSlide{i}.xml', 'application/vnd.openxmlformats-officedocument.presentationml.notesSlide+xml'))
    body = ''.join(f'<Override PartName="{p}" ContentType="{c}"/>' for p, c in overrides)
    return ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
            '<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">'
            '<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>'
            '<Default Extension="xml" ContentType="application/xml"/>' + body + '</Types>')


def theme_xml() -> str:
    return '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<a:theme xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" name="TSSE Evidence">
 <a:themeElements><a:clrScheme name="TSSE">
  <a:dk1><a:srgbClr val="102A43"/></a:dk1><a:lt1><a:srgbClr val="F4F1EA"/></a:lt1>
  <a:dk2><a:srgbClr val="243B53"/></a:dk2><a:lt2><a:srgbClr val="FFFFFF"/></a:lt2>
  <a:accent1><a:srgbClr val="008C8C"/></a:accent1><a:accent2><a:srgbClr val="2F6B9A"/></a:accent2>
  <a:accent3><a:srgbClr val="E76F51"/></a:accent3><a:accent4><a:srgbClr val="2E7D62"/></a:accent4>
  <a:accent5><a:srgbClr val="627D98"/></a:accent5><a:accent6><a:srgbClr val="CAD5DF"/></a:accent6>
  <a:hlink><a:srgbClr val="0563C1"/></a:hlink><a:folHlink><a:srgbClr val="954F72"/></a:folHlink>
 </a:clrScheme><a:fontScheme name="TSSE"><a:majorFont><a:latin typeface="Arial"/><a:ea typeface=""/><a:cs typeface=""/></a:majorFont><a:minorFont><a:latin typeface="Arial"/><a:ea typeface=""/><a:cs typeface=""/></a:minorFont></a:fontScheme>
 <a:fmtScheme name="TSSE"><a:fillStyleLst><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:fillStyleLst><a:lnStyleLst><a:ln w="9525"><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:ln></a:lnStyleLst><a:effectStyleLst><a:effectStyle><a:effectLst/></a:effectStyle></a:effectStyleLst><a:bgFillStyleLst><a:solidFill><a:schemeClr val="phClr"/></a:solidFill></a:bgFillStyleLst></a:fmtScheme>
 </a:themeElements><a:objectDefaults/><a:extraClrSchemeLst/></a:theme>'''


def empty_group() -> str:
    return ('<p:spTree><p:nvGrpSpPr><p:cNvPr id="1" name=""/><p:cNvGrpSpPr/><p:nvPr/></p:nvGrpSpPr>'
            '<p:grpSpPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="0" cy="0"/><a:chOff x="0" y="0"/><a:chExt cx="0" cy="0"/></a:xfrm></p:grpSpPr></p:spTree>')


def master_xml() -> str:
    return ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
            '<p:sldMaster xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">'
            f'<p:cSld name="TSSE Master">{empty_group()}</p:cSld><p:clrMap accent1="accent1" accent2="accent2" accent3="accent3" accent4="accent4" accent5="accent5" accent6="accent6" bg1="lt1" bg2="lt2" folHlink="folHlink" hlink="hlink" tx1="dk1" tx2="dk2"/>'
            '<p:sldLayoutIdLst><p:sldLayoutId id="1" r:id="rId1"/></p:sldLayoutIdLst><p:txStyles><p:titleStyle><a:lvl1pPr algn="l"><a:defRPr sz="3000" b="1"/></a:lvl1pPr></p:titleStyle><p:bodyStyle><a:lvl1pPr><a:defRPr sz="1800"/></a:lvl1pPr></p:bodyStyle><p:otherStyle><a:defPPr><a:defRPr lang="es-AR"/></a:defPPr></p:otherStyle></p:txStyles></p:sldMaster>')


def layout_xml() -> str:
    return ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
            '<p:sldLayout xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main" type="blank" preserve="1">'
            f'<p:cSld name="Blank">{empty_group()}</p:cSld><p:clrMapOvr><a:masterClrMapping/></p:clrMapOvr></p:sldLayout>')


def notes_master_xml() -> str:
    return ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
            '<p:notesMaster xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">'
            f'<p:cSld name="Notes Master">{empty_group()}</p:cSld><p:clrMap accent1="accent1" accent2="accent2" accent3="accent3" accent4="accent4" accent5="accent5" accent6="accent6" bg1="lt1" bg2="lt2" folHlink="folHlink" hlink="hlink" tx1="dk1" tx2="dk2"/><p:hf dt="0" hdr="0" ftr="0" sldNum="0"/><p:notesStyle><a:lvl1pPr><a:defRPr sz="1200"/></a:lvl1pPr></p:notesStyle></p:notesMaster>')


def notes_slide_xml(note: str) -> str:
    paras = ''.join(paragraph(line or " ", 11, INK, False) for line in note.splitlines())
    return ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
            '<p:notes xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">'
            '<p:cSld><p:spTree><p:nvGrpSpPr><p:cNvPr id="1" name=""/><p:cNvGrpSpPr/><p:nvPr/></p:nvGrpSpPr><p:grpSpPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="0" cy="0"/><a:chOff x="0" y="0"/><a:chExt cx="0" cy="0"/></a:xfrm></p:grpSpPr>'
            '<p:sp><p:nvSpPr><p:cNvPr id="2" name="Notes Body"/><p:cNvSpPr txBox="1"/><p:nvPr><p:ph type="body" idx="1"/></p:nvPr></p:nvSpPr><p:spPr><a:xfrm><a:off x="457200" y="2743200"/><a:ext cx="8229600" cy="5486400"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:noFill/><a:ln><a:noFill/></a:ln></p:spPr>'
            f'<p:txBody><a:bodyPr/><a:lstStyle/>{paras}</p:txBody></p:sp></p:spTree></p:cSld><p:clrMapOvr><a:masterClrMapping/></p:clrMapOvr></p:notes>')


def relationships(items: list[tuple[str, str, str]]) -> str:
    body = ''.join(f'<Relationship Id="{rid}" Type="{typ}" Target="{target}"/>' for rid, typ, target in items)
    return ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
            '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">' + body + '</Relationships>')


def write_entry(zf: zipfile.ZipFile, name: str, text: str) -> None:
    info = zipfile.ZipInfo(name, FIXED_ZIP_TIME)
    info.compress_type = zipfile.ZIP_DEFLATED
    info.external_attr = 0o644 << 16
    zf.writestr(info, text.encode("utf-8"))


def generate(output: Path) -> tuple[int, str]:
    slides, titles, notes = build_slides()
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, "w") as zf:
        write_entry(zf, "[Content_Types].xml", content_types(len(slides)))
        write_entry(zf, "_rels/.rels", relationships([
            ("rId1", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument", "ppt/presentation.xml"),
            ("rId2", "http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties", "docProps/core.xml"),
            ("rId3", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties", "docProps/app.xml"),
        ]))
        slide_ids = ''.join(f'<p:sldId id="{255+i}" r:id="rId{6+i}"/>' for i in range(1, len(slides)+1))
        presentation = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
                        '<p:presentation xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main">'
                        f'<p:sldMasterIdLst><p:sldMasterId id="2147483648" r:id="rId1"/></p:sldMasterIdLst><p:notesMasterIdLst><p:notesMasterId r:id="rId5"/></p:notesMasterIdLst><p:sldIdLst>{slide_ids}</p:sldIdLst>'
                        f'<p:sldSz cx="{emu(W)}" cy="{emu(H)}" type="screen16x9"/><p:notesSz cx="6858000" cy="9144000"/><p:defaultTextStyle><a:defPPr><a:defRPr lang="es-AR"/></a:defPPr></p:defaultTextStyle></p:presentation>')
        write_entry(zf, "ppt/presentation.xml", presentation)
        pres_rels = [
            ("rId1", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideMaster", "slideMasters/slideMaster1.xml"),
            ("rId2", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/presProps", "presProps.xml"),
            ("rId3", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/viewProps", "viewProps.xml"),
            ("rId4", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/tableStyles", "tableStyles.xml"),
            ("rId5", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/notesMaster", "notesMasters/notesMaster1.xml"),
        ] + [(f"rId{6+i}", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/slide", f"slides/slide{i}.xml") for i in range(1, len(slides)+1)]
        write_entry(zf, "ppt/_rels/presentation.xml.rels", relationships(pres_rels))
        write_entry(zf, "ppt/theme/theme1.xml", theme_xml())
        write_entry(zf, "ppt/slideMasters/slideMaster1.xml", master_xml())
        write_entry(zf, "ppt/slideMasters/_rels/slideMaster1.xml.rels", relationships([
            ("rId1", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideLayout", "../slideLayouts/slideLayout1.xml"),
            ("rId2", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme", "../theme/theme1.xml"),
        ]))
        write_entry(zf, "ppt/slideLayouts/slideLayout1.xml", layout_xml())
        write_entry(zf, "ppt/slideLayouts/_rels/slideLayout1.xml.rels", relationships([
            ("rId1", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideMaster", "../slideMasters/slideMaster1.xml"),
        ]))
        write_entry(zf, "ppt/notesMasters/notesMaster1.xml", notes_master_xml())
        write_entry(zf, "ppt/notesMasters/_rels/notesMaster1.xml.rels", relationships([
            ("rId1", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme", "../theme/theme1.xml"),
        ]))
        for i, (slide, note) in enumerate(zip(slides, notes), 1):
            write_entry(zf, f"ppt/slides/slide{i}.xml", slide.xml())
            write_entry(zf, f"ppt/slides/_rels/slide{i}.xml.rels", relationships([
                ("rId1", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideLayout", "../slideLayouts/slideLayout1.xml"),
                ("rId2", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/notesSlide", f"../notesSlides/notesSlide{i}.xml"),
            ]))
            write_entry(zf, f"ppt/notesSlides/notesSlide{i}.xml", notes_slide_xml(note))
            write_entry(zf, f"ppt/notesSlides/_rels/notesSlide{i}.xml.rels", relationships([
                ("rId1", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/notesMaster", "../notesMasters/notesMaster1.xml"),
                ("rId2", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/slide", f"../slides/slide{i}.xml"),
            ]))
        write_entry(zf, "ppt/presProps.xml", '<?xml version="1.0" encoding="UTF-8" standalone="yes"?><p:presentationPr xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main"/>')
        write_entry(zf, "ppt/viewProps.xml", '<?xml version="1.0" encoding="UTF-8" standalone="yes"?><p:viewPr xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main" lastView="sldView"><p:normalViewPr/><p:slideViewPr/><p:notesTextViewPr/></p:viewPr>')
        write_entry(zf, "ppt/tableStyles.xml", '<?xml version="1.0" encoding="UTF-8" standalone="yes"?><a:tblStyleLst xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" def="{5C22544A-7EE6-4342-B048-85BDC9FD1C3A}"/>')
        title_vector = ''.join(f'<vt:lpstr>{xml_text(t)}</vt:lpstr>' for t in titles)
        app = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
               '<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/extended-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">'
               f'<Application>Python standard library OOXML generator</Application><PresentationFormat>On-screen Show (16:9)</PresentationFormat><Slides>{len(slides)}</Slides><Notes>{len(slides)}</Notes><HiddenSlides>0</HiddenSlides><TitlesOfParts><vt:vector size="{len(slides)}" baseType="lpstr">{title_vector}</vt:vector></TitlesOfParts></Properties>')
        write_entry(zf, "docProps/app.xml", app)
        core = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
                '<cp:coreProperties xmlns:cp="http://schemas.openxmlformats.org/package/2006/metadata/core-properties" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:dcterms="http://purl.org/dc/terms/" xmlns:dcmitype="http://purl.org/dc/dcmitype/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">'
                '<dc:title>Verificación de software para adquisición ADS1263</dc:title><dc:subject>TSSE Milestone 4</dc:subject><dc:creator>IdS project</dc:creator><cp:keywords>TSSE,CFT,unit testing,integration,coverage,static analysis</cp:keywords><dc:description>Deck principal de 9 láminas más 7 láminas de respaldo.</dc:description><cp:lastModifiedBy>Deterministic Python generator</cp:lastModifiedBy><cp:revision>1</cp:revision><dcterms:created xsi:type="dcterms:W3CDTF">2026-10-15T16:12:03Z</dcterms:created><dcterms:modified xsi:type="dcterms:W3CDTF">2026-10-15T16:12:03Z</dcterms:modified></cp:coreProperties>')
        write_entry(zf, "docProps/core.xml", core)
    digest = hashlib.sha256(output.read_bytes()).hexdigest()
    return len(slides), digest


def verify(path: Path) -> list[str]:
    errors: list[str] = []
    required = {"[Content_Types].xml", "_rels/.rels", "ppt/presentation.xml", "ppt/_rels/presentation.xml.rels", "ppt/slideMasters/slideMaster1.xml", "ppt/slideLayouts/slideLayout1.xml", "ppt/notesMasters/notesMaster1.xml"}
    try:
        with zipfile.ZipFile(path) as zf:
            bad = zf.testzip()
            if bad:
                errors.append(f"CRC failure: {bad}")
            names = set(zf.namelist())
            missing = required - names
            if missing:
                errors.append(f"Missing required parts: {sorted(missing)}")
            xml_names = sorted(n for n in names if n.endswith(".xml") or n.endswith(".rels"))
            for name in xml_names:
                try:
                    ET.fromstring(zf.read(name))
                except ET.ParseError as exc:
                    errors.append(f"XML parse error in {name}: {exc}")
            rel_ns = {"r": "http://schemas.openxmlformats.org/package/2006/relationships"}
            for name in sorted(n for n in names if n.endswith(".rels")):
                root = ET.fromstring(zf.read(name))
                if name == "_rels/.rels":
                    base = ""
                else:
                    base = posixpath.dirname(posixpath.dirname(name))
                for rel in root.findall("r:Relationship", rel_ns):
                    if rel.get("TargetMode") == "External":
                        continue
                    target = posixpath.normpath(posixpath.join(base, rel.get("Target", "")))
                    if target not in names:
                        errors.append(f"Missing relationship target: {name} -> {target}")
            slides = sorted(n for n in names if n.startswith("ppt/slides/slide") and n.endswith(".xml"))
            notes = sorted(n for n in names if n.startswith("ppt/notesSlides/notesSlide") and n.endswith(".xml"))
            if len(slides) != 16:
                errors.append(f"Expected 16 slides, found {len(slides)}")
            if len(notes) != 16:
                errors.append(f"Expected 16 note slides, found {len(notes)}")
            drawing_ns = {"a": "http://schemas.openxmlformats.org/drawingml/2006/main"}
            slide_text = "\n".join(
                node.text or ""
                for name in slides
                for node in ET.fromstring(zf.read(name)).findall(".//a:t", drawing_ns)
            )
            required_claims = [
                "72 / 72", "24 / 24", "71 / 71", "151/222", "13/16", "90/134",
                "118/168", "9/13", "53/76", "54/168", "6/13", "26/76",
                "66 ADS1263 + 6 robustez acquisition", "casos CFT PASS",
                "41", "37", "unusedStructMember", "constParameterPointer", "staticFunction",
                "32", "missingIncludeSystem", "BLOCKED", "DEF-001", "VALIDACIÓN EN PLACA", "ROB-001", "ROB-003",
            ]
            for claim in required_claims:
                if claim not in slide_text:
                    errors.append(f"Missing required deck claim: {claim}")
            for stale_claim in ["96/96", "96 unitarias", "36 unusedStructMember"]:
                if stale_claim in slide_text:
                    errors.append(f"Stale deck claim present: {stale_claim}")
            for name in notes:
                note_text = " ".join(
                    node.text or "" for node in ET.fromstring(zf.read(name)).findall(".//a:t", drawing_ns)
                )
                if "docs/" not in note_text:
                    errors.append(f"Speaker note lacks repository evidence path: {name}")
            p_ns = {"p": "http://schemas.openxmlformats.org/presentationml/2006/main"}
            pres_root = ET.fromstring(zf.read("ppt/presentation.xml"))
            size = pres_root.find("p:sldSz", p_ns)
            if size is None or size.get("cx") != "12192000" or size.get("cy") != "6858000":
                errors.append("Presentation size is not exact 16:9 widescreen OOXML dimensions")
    except (OSError, zipfile.BadZipFile) as exc:
        errors.append(f"ZIP error: {exc}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, default=Path(__file__).with_name(OUT_NAME))
    parser.add_argument("--verify-only", action="store_true")
    args = parser.parse_args()
    if args.verify_only:
        errors = verify(args.output)
        if errors:
            for item in errors:
                print(f"ERROR: {item}")
            return 1
        digest = hashlib.sha256(args.output.read_bytes()).hexdigest()
        with zipfile.ZipFile(args.output) as zf:
            print(f"PASS ZIP CRC: {len(zf.namelist())} parts")
            print("PASS XML parse: all XML and relationship parts")
            print("PASS relationships: all internal targets present")
            print("PASS OOXML inventory: 16 slides + 16 note slides")
            print("PASS content assertions: required claims and evidence-linked notes")
            print("PASS page size: 12192000 x 6858000 EMU (16:9)")
        print(f"SHA256 {digest}  {args.output.as_posix()}")
        return 0
    count, digest = generate(args.output)
    errors = verify(args.output)
    if errors:
        for item in errors:
            print(f"ERROR: {item}", file=sys.stderr)
        return 1
    print(f"Generated {args.output.as_posix()}")
    print(f"Slides: {count} (9 main + 7 backup); speaker notes: {count}")
    print(f"SHA256: {digest}")
    print("Verification: PASS (ZIP CRC, required parts, XML parse, slide/note counts)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
