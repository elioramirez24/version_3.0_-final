# Convierte circuito_real_v3.html en ../circuito_real_v3.png.
# Requiere: pip install playwright  y Google Chrome (o: playwright install chromium y quitar executable_path).
import asyncio, os
from playwright.async_api import async_playwright

async def main():
    src = os.path.abspath("circuito_real_v3.html")
    async with async_playwright() as p:
        b = await p.chromium.launch(executable_path="/usr/bin/google-chrome", args=["--no-sandbox"])
        pg = await b.new_page(viewport={"width": 1840, "height": 1200}, device_scale_factor=1.45)
        await pg.goto("file://" + src)
        await pg.wait_for_timeout(400)
        await (await pg.query_selector(".wrap")).screenshot(path="../circuito_real_v3.png")
        await b.close()

asyncio.run(main())
