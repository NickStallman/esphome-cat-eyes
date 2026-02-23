import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome import pins

cat_eyes_ns = cg.esphome_ns.namespace("cat_eyes")
CatEyes = cat_eyes_ns.class_("CatEyes", cg.Component)

CONF_CLK_PIN = "clk_pin"
CONF_MOSI_PIN = "mosi_pin"
CONF_CS_RIGHT = "cs_right"
CONF_CS_LEFT = "cs_left"
CONF_DC_PIN = "dc_pin"
CONF_RESET_PIN = "reset_pin"
CONF_SPI_FREQUENCY = "spi_frequency"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CatEyes),
    cv.Required(CONF_CLK_PIN): pins.internal_gpio_output_pin_number,
    cv.Required(CONF_MOSI_PIN): pins.internal_gpio_output_pin_number,
    cv.Required(CONF_CS_RIGHT): pins.internal_gpio_output_pin_number,
    cv.Required(CONF_CS_LEFT): pins.internal_gpio_output_pin_number,
    cv.Required(CONF_DC_PIN): pins.internal_gpio_output_pin_number,
    cv.Required(CONF_RESET_PIN): pins.internal_gpio_output_pin_number,
    cv.Optional(CONF_SPI_FREQUENCY, default=80000000): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_clk_pin(config[CONF_CLK_PIN]))
    cg.add(var.set_mosi_pin(config[CONF_MOSI_PIN]))
    cg.add(var.set_cs_right(config[CONF_CS_RIGHT]))
    cg.add(var.set_cs_left(config[CONF_CS_LEFT]))
    cg.add(var.set_dc_pin(config[CONF_DC_PIN]))
    cg.add(var.set_reset_pin(config[CONF_RESET_PIN]))
    cg.add(var.set_spi_frequency(config[CONF_SPI_FREQUENCY]))
