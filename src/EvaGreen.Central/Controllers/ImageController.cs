using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace EvaGreen.Central.Controllers
{
    public sealed class Image
    {
        public string Name { get; set; }
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
                    Name = "hello.jpg",
                }
            });
        }
    }
}
