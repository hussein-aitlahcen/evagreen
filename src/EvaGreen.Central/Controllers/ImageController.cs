using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace EvaGreen.Central.Controllers
{
    public sealed class Image
    {
        public string Name { get; set; }
        public string UniqueId { get; set; }
    }

    [Authorize]
    [Route("api/[controller]")]
    public sealed class ImageController : Controller
    {
        [HttpGet]
        public IActionResult Get()
        {
            return Json(new[]
            {
                new Image
                {
                    UniqueId = "01_01_01",
                    Name = "hello.jpg",
                }
            });
        }
    }
}
