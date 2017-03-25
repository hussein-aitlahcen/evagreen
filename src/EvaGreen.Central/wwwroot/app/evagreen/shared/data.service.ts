import { Injectable } from '@angular/core';
import { AuthHttp } from 'angular2-jwt';
import { Observable } from 'rxjs/Observable';


export class Data {

    public static DATA_IMAGE = 0x01;
    public static DATA_TEMPERATURE = 0x02;
    public static DATA_VIDEO = 0xFF

    public id: Number;
    public agentId: Number;
    public type: Number;
    public creationDate: Number;
    public integrationDate: Number;
    public value: string;
    public description: string;

    constructor(json: any) {
        this.id = json.id;
        this.agentId = json.agentId;
        this.type = json.type;
        this.creationDate = json.creationDate;
        this.integrationDate = json.integrationDate;
        this.value = json.value;
        this.description = json.description;
    }

    getCreationTime(): string {
        return this.getTimeDifference(new Date(this.creationDate.valueOf() * 1000));
    }

    getIntegrationTime(): string {
        return this.getTimeDifference(new Date(this.integrationDate.valueOf() * 1000));
    }

    getTimeDifference(previous: Date): string {

        var current = new Date();
        var msPerMinute = 60 * 1000;
        var msPerHour = msPerMinute * 60;
        var msPerDay = msPerHour * 24;
        var msPerMonth = msPerDay * 30;
        var msPerYear = msPerDay * 365;

        var elapsed = (current.valueOf() - previous.valueOf());

        if (elapsed < msPerMinute) {
            return "il y à " + Math.round(elapsed / 1000) + " secondes";
        }
        else if (elapsed < msPerHour) {
            return "il y à " + Math.round(elapsed / msPerMinute) + " minutes";
        }
        else if (elapsed < msPerDay) {
            return "il y à " + Math.round(elapsed / msPerHour) + " heures";
        }
        else if (elapsed < msPerMonth) {
            return "il y à " + Math.round(elapsed / msPerDay) + " jours";
        }
        else if (elapsed < msPerYear) {
            return "il y à " + Math.round(elapsed / msPerMonth) + " mois";
        }
        else {
            return "il y à " + Math.round(elapsed / msPerYear) + " ans";
        }
    }
}

@Injectable()
export class DataService {
    constructor(private authHttp: AuthHttp) { }

    all(agentId: Number): Observable<Data[]> {
        return this.authHttp.get("/api/data/agent/" + agentId)
            .map(res => res.json().map(obj => new Data(obj)));
    }
}