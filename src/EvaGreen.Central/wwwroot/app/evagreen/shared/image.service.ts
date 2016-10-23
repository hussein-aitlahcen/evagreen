import { Injectable } from '@angular/core';
import { AuthHttp } from 'angular2-jwt';
import { Observable } from 'rxjs/Observable';

export class Image {
    constructor(public name: String) {}
}

@Injectable()
export class ImageService {
    constructor(private authHttp: AuthHttp) {}
    
    all(): Observable<Image[]> {
        return this.authHttp.get("/api/image")


            .map(res => res.json());
    }
}