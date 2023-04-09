#include <vector>

#include <drogon/drogon.h>
#include <drogon/HttpAppFramework.h>

using namespace drogon;

int main()
{
    app().setLogLevel(trantor::Logger::kTrace);
    app().registerHandler("/",
        [](const HttpRequestPtr &,
           std::function<void(const HttpResponsePtr &)> &&callback) {
            std::string uploadPage = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>File upload</title><script type=\"text/javascript\">"
        "var xhr;"
        "function UploadFile() {var fileObj = document.getElementById(\"file\").files[0];"
            "var url = \"/upload_endpoint\";" 
            "var form = new FormData();"
            "form.append(\"file\", fileObj);"
            "xhr = new XMLHttpRequest();"
            "xhr.open(\"post\", url, true);"
            "xhr.onload = uploadComplete;"
            "xhr.onerror = uploadFailed;"
            "xhr.upload.onprogress = progressFunction;"
            "xhr.upload.onloadstart = function(){ot = new Date().getTime();"
                "oloaded = 0;};"
            "xhr.send(form);}"
        "function uploadComplete(evt) {var data = evt.target.responseText;"
            "alert(\"File has been uploaded.\" + data);}"
        "function uploadFailed(evt) {alert(\"Upload failed!\");}"
        "function cancelUploadFile(){xhr.abort();}"
        "function progressFunction(evt) { var progressBar = document.getElementById(\"progressBar\");"
            "var percentageDiv = document.getElementById(\"percentage\");"
            "if (evt.lengthComputable) {progressBar.max = evt.total;"
                "progressBar.value = evt.loaded;"
                "percentageDiv.innerHTML = Math.round(evt.loaded / evt.total * 100) + \"%\";}"
            "var time = document.getElementById(\"time\");"
            "var nt = new Date().getTime();"
            "var pertime = (nt-ot)/1000;"
            "ot = new Date().getTime();"
            "var perload = evt.loaded - oloaded;"
            "oloaded = evt.loaded;"
            "var speed = perload/pertime;"
            "var bspeed = speed;"
            "var units = 'b/s';"
            "if(speed/1024>1){speed = speed/1024;"
                "units = 'k/s';}"
            "if(speed/1024>1){speed = speed/1024;"
                "units = 'M/s';}"
            "speed = speed.toFixed(1);"
            "var resttime = ((evt.total-evt.loaded)/bspeed).toFixed(1);"
            "time.innerHTML = ',Speed: '+speed+units+', the remaining time: '+resttime+'s';"
            "if(bspeed==0) time.innerHTML = 'Upload cancelled';}"
    "</script></head><body><progress id=\"progressBar\" value=\"0\" max=\"100\" style=\"width: 300px;\"></progress><span id=\"percentage\"></span><span id=\"time\"></span>"
    "<br /><br /><input type=\"file\" id=\"file\" name=\"myfile\" /><input type=\"button\" onclick=\"UploadFile()\" value=\"Upload\" /><input type=\"button\" onclick=\"cancelUploadFile()\" value=\"Cancel\" /></body></html>";
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(uploadPage);
            callback(resp);
        });

    app().registerHandler("/upload_endpoint",
        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback) {
            MultiPartParser fileUpload;
            if (fileUpload.parse(req) != 0 || fileUpload.getFiles().size() != 1)
            {
                auto resp = HttpResponse::newHttpResponse();
                resp->setBody("Must only be one file");
                resp->setStatusCode(k403Forbidden);
                callback(resp);
                return;
            }

            auto &file = fileUpload.getFiles()[0];
            auto md5 = file.getMd5();
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(
                "The server has calculated the file's MD5 hash to be " + md5);
            file.save();
            LOG_INFO << "The uploaded file has been saved to the ./uploads directory";
            callback(resp);
        },
        {Post});

    LOG_INFO << "Server running on 127.0.0.1:8848";
    app()
        .setClientMaxBodySize(20 * 2000 * 2000)
        .setUploadPath("./uploads")
        .addListener("127.0.0.1", 8848)
        .run();
}