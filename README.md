# Boilerplate Code and Documentation

At Backend there are often boilerplate code and documentation that could be generated and kept almost automatically updated. That might spare not only a lot of time/pain when it comes down to maintainance and testing, but also produce better documentation for Frontend/Customer parties. 

![Boilerplate](/images/boilerplate.jpeg)

## Decouple User Output

Humble attempt to dissociate Backend generated data/schemas from friendly user Frontend formats

<!--

@startuml diagram.png

top to bottom direction
skinparam packageStyle rectangle

actor "Frontend\n...Dev..." as FD
actor "Backend\n...Dev..." as BD
actor Customer 

note bottom of BD: Foggier idea of Customer\nformat preferencies\n\nAimed at making Fronted Dev life easier
note top of FD: Foggier idea of ugliness\nin backend data\n\nAimed at making Customer life easier
note top of Customer: Don't care of Backend or Frontend issues\n\nBusy making money

rectangle BackendServer {
(file\nread) .> (structured\ndata)
(database\nqueries) .> (structured\ndata)
BD -- (adding\ngeneric\nformat info)
(structured\ndata) .> (adding\ngeneric\nformat info)
(adding\ngeneric\nformat info) .> (raw data\nwith generic\nformat info)
}

database DataBase {
(dataX) .> (database\nqueries)
}
cloud FileSystem {
(dataY) .> (file\nread)
}


rectangle FrontendServer {
(raw data\nwith generic\nformat info) .> (choose specific\noutput format)
(user output) .> Customer
(apply chosen format) .> (user output)
(choose specific\noutput format) .> (apply chosen format)
FD -- (choose specific\noutput format)
}
@enduml

-->
![Diagram](/images/diagram.png)

### Example: Tech Json Schema Info into User-Friendly HTML table

Customer is more used to **read** HTML than hairy, lengthy *Json Schema* raw files. So we'd better to provided that expected format in order to get some **constructive feedback** from them.

<!--

@startuml json.png

top to bottom direction
skinparam packageStyle rectangle

actor "Frontend\n...Dev..." as FD
actor "Backend\n...Dev..." as BD
actor Customer 

rectangle BackendServer {
(file\nread) .> (structured\ndata)
BD -- (adding\ngeneric\nformat info)
(structured\ndata) .> (adding\ngeneric\nformat info)
(adding\ngeneric\nformat info) .> (SIMPLY HTML)
}

cloud FileSystem {
(Json Schema) .> (file\nread)
}


rectangle FrontendServer {
(SIMPLY HTML) .> (choose specific\nCSS)
(user output\nHTML\CSS) .> Customer
(apply chosen format) .> (user output\nHTML\CSS)
(choose specific\nCSS) .> (apply chosen format)
FD -- (choose specific\nCSS)
}
@enduml

-->
![Json Example](/images/json.png)

## Useful libraries

At the **backend** [Boost Hana](https://github.com/boostorg/hana) by [Luois Dionne](https://github.com/ldionne) and [RapidJSON](https://github.com/miloyip/rapidjson) by [Milo Yip](https://github.com/miloyip) can be used to fight back all the ugliness possible and get to the **frontend** some simplified *Markdown/HTML/text* data to transform through [Pandoc](http://pandoc.org/) into waterdowned *HTML/CSS*.
